#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arena.c"
#include "arena.h"

typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t i32;

typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t i32;

#define ARENA_SIZE 4096
#define INPUT_BUFFER_SIZE 256

/*  Enums  */
/*  =====  */

enum TokenType {
    Token_Error = 0,
    Token_Number,
    Token_Star,
    Token_Slash,
    Token_Plus,
    Token_Caret,
    Token_Minus,
    Token_ParensOpen,
    Token_ParensClose,
    Token_Max
};

typedef u32 ExpressionNodeKind;
enum {
    NodeType_Error,
    NodeType_Number,
    NodeType_Add,
    NodeType_Sub,
    NodeType_Mul,
    NodeType_Div,
    NodeType_Pow,
    NodeType_Positive,
    NodeType_Negative,
};

typedef u32 Precedence;
enum {
    Precedence_Min,
    Precedence_Term,
    Precedence_Factor,
    Precedence_Power,
};

/*  Structs  */
/*  =======  */

typedef struct string {
    char *chars;
    u32 len;
} String;

typedef struct token {
    enum TokenType type;
    u32 start;
} Token;

typedef struct Parser {
    Token *tokenlist;
    Token current;
    u32 curr_token_idx;
    String str;
} Parser;

typedef struct ExpressionNode ExpressionNode;
struct ExpressionNode {
    ExpressionNodeKind type;

    union {
        f64 number;
        struct {
            ExpressionNode *operand;
        } unary;
        struct {
            ExpressionNode *left;
            ExpressionNode *right;
        } binary;
    };
};

/*  Prototypes  */
/*  ==========  */
String StringNew(char *chars);
int StringCompare(String str1, String str2);
i32 get_token_number(const String *str, i32 offset);
void tokenize(Token *token_list, const String str);
f64 evaluate(ExpressionNode *node);
f64 number_from_token(Token token, const String *str);
void parser_advance(Parser *parser);
f64 number_from_token(Token token, const String *str);
ExpressionNode *
parser_parse_expression(Arena *arena, Parser *parser, Precedence prev_prec);
ExpressionNode *parser_parse_infix_expr(
    Arena *arena, Parser *parser, Token operator, ExpressionNode * left);
void pretty_print_expression_node(const ExpressionNode *node, int indent_level);
void print_indent(u32 indent_level);

/*  Globals  */
/*  =======  */
static Precedence precedence_lookup[Token_Max] = {
    [Token_Plus]  = Precedence_Term,
    [Token_Minus] = Precedence_Term,
    [Token_Slash] = Precedence_Factor,
    [Token_Star]  = Precedence_Factor,
    [Token_Caret] = Precedence_Power,
};

/*               Main              */
/*  =============================  */
i32 main(int argc, char *argv[])
{
    /*printf("argument count: %d\n", argc);*/
    Arena arena;
    u32 index = 0;

    do {
        String argument = StringNew(argv[index]);

        /*printf("arg%d: %s\n", i, argument.chars);*/
        if (StringCompare(argument, StringNew("-h"))) {
            printf("Usage:\ncalc <no args>:\n\ttype your expression and press "
                   "enter.\n");
            printf("\ncalc -i 'expression':\n\tcomputes the entered expression "
                   "and quits");
            return 0;
        }
    } while (++index < argc);

    index             = 0;
    String expression = {};
    do {
        String argument = StringNew(argv[index]);
        if (StringCompare(argument, StringNew("-i"))) {
            expression = StringNew(argv[index + 1]);
            break;
        }
    } while (++index < argc);

    if (!(expression.chars == NULL)) {
        arena_init(&arena, ARENA_SIZE);
        printf("\n%s =", expression.chars);

        Token *token_list =
            (Token *)arena_alloc(&arena, expression.len * sizeof(Token));
        tokenize(token_list, expression);

        Parser parser = {
            .tokenlist      = token_list,
            .current        = token_list[0],
            .curr_token_idx = 0,
            .str            = expression};

        ExpressionNode *node_root =
            parser_parse_expression(&arena, &parser, Precedence_Min);
        /*pretty_print_expression_node(node_root, 0);*/

        f64 result = evaluate(node_root);
        printf("%.3e \n", result);

        /*clean up*/
        arena_free(&arena);
        return 0;
    }

    arena_init(&arena, ARENA_SIZE);
    char input[INPUT_BUFFER_SIZE];
    printf("\x1b[32mPlease input your expression: \x1b[0m");
    fgets(input, INPUT_BUFFER_SIZE, stdin);
    input[strcspn(input, "\n")] = 0;

    printf("%s = ", input);
    String str        = StringNew(input);
    Token *token_list = (Token *)arena_alloc(&arena, str.len * sizeof(Token));
    tokenize(token_list, str);

    Parser parser = {
        .tokenlist      = token_list,
        .current        = token_list[0],
        .curr_token_idx = 0,
        .str            = str};

    ExpressionNode *node_root =
        parser_parse_expression(&arena, &parser, Precedence_Min);
    /*pretty_print_expression_node(node_root, 0);*/

    f64 result = evaluate(node_root);
    printf("\x1b[4m\x1b[33m%.3e\x1b[0m\n", result);

    /*clean up*/
    arena_free(&arena);
    return 0;
}
/*====================================================== */

f64 evaluate(ExpressionNode *node)
{
    switch (node->type) {
        case NodeType_Number:
            return node->number;
            break;

        case NodeType_Add:
            return evaluate(node->binary.left) + evaluate(node->binary.right);
        case NodeType_Div:
            return evaluate(node->binary.left) / evaluate(node->binary.right);
        case NodeType_Mul:
            return evaluate(node->binary.left) * evaluate(node->binary.right);
        case NodeType_Sub:
            return evaluate(node->binary.left) - evaluate(node->binary.right);
        case NodeType_Positive:
            return evaluate(node->unary.operand);
        case NodeType_Negative:
            return -evaluate(node->unary.operand);
        case NodeType_Pow:
            return pow(
                evaluate(node->binary.left), evaluate(node->binary.right));
    }
    return 0.0;
}
/*====================================================== */

void parser_advance(Parser *parser)
{
    parser->curr_token_idx++;
    parser->current = parser->tokenlist[parser->curr_token_idx];
}
/*====================================================== */

f64 number_from_token(Token token, const String *str)
{
    char *start = &str->chars[token.start];
    char *endptr;
    f64 result;

    result = strtod(start, &endptr);

    if (endptr == start) {
        fprintf(
            stderr, "Error: No conversion performed for string '%s'.\n", start);
        return 0.0f;
    }
    return result;
}
/*====================================================== */

ExpressionNode *
parser_parse_number(Arena *arena, Parser *parser, const String *str)
{
    ExpressionNode *node = arena_alloc(arena, sizeof(ExpressionNode));
    node->type           = NodeType_Number;
    node->number         = number_from_token(parser->current, str);
    parser_advance(parser);
    return node;
}
/*====================================================== */

ExpressionNode *parser_parse_infix_expr(
    Arena *arena, Parser *parser, Token operator, ExpressionNode * left)
{
    ExpressionNode *ret_node = arena_alloc(arena, sizeof(ExpressionNode));
    switch (operator.type) {
        case Token_Plus:
            ret_node->type = NodeType_Add;
            break;

        case Token_Minus:
            ret_node->type = NodeType_Sub;
            break;

        case Token_Star:
            ret_node->type = NodeType_Mul;
            break;

        case Token_Slash:
            ret_node->type = NodeType_Div;
            break;

        case Token_Caret:
            ret_node->type = NodeType_Pow;
            break;

        default:
            printf(
                "ParseException. Got wrong token %d at position %d!\n",
                operator.type,
                operator.start);
    }

    ret_node->binary.left = left;
    ret_node->binary.right = parser_parse_expression(arena, parser, precedence_lookup[operator.type]);
    return ret_node;
}
/*====================================================== */

ExpressionNode *parser_parse_terminal_expr(Arena *arena, Parser *parser);
ExpressionNode *parser_parse_terminal_expr(Arena *arena, Parser *parser)
{
    ExpressionNode *ret_node = 0;
    if (parser->current.type == Token_Number) {
        ret_node = parser_parse_number(arena, parser, &parser->str);
    } else if (parser->current.type == Token_ParensOpen) {
        parser_advance(parser);
        ret_node = parser_parse_expression(arena, parser, Precedence_Min);
        if (parser->current.type == Token_ParensClose) {
            parser_advance(parser);
        }
    } else if (parser->current.type == Token_Plus) {
        parser_advance(parser);
        ret_node                = arena_alloc(arena, sizeof(ExpressionNode));
        ret_node->type          = NodeType_Positive;
        ret_node->unary.operand = parser_parse_terminal_expr(arena, parser);
    } else if (parser->current.type == Token_Minus) {
        parser_advance(parser);
        ret_node                = arena_alloc(arena, sizeof(ExpressionNode));
        ret_node->type          = NodeType_Negative;
        ret_node->unary.operand = parser_parse_terminal_expr(arena, parser);
    }
    return ret_node;
}
/*====================================================== */

ExpressionNode *
parser_parse_expression(Arena *arena, Parser *parser, Precedence prev_prec)
{
    /*Recurvise*/
    ExpressionNode *left = parser_parse_terminal_expr(arena, parser);
    Token curr_operator  = parser->current;
    Precedence curr_prec = precedence_lookup[curr_operator.type];
    while (Precedence_Min != curr_prec) {
        if (prev_prec >= curr_prec) {
            break;
        } else {
            parser_advance(parser);
            left = parser_parse_infix_expr(arena, parser, curr_operator, left);
            curr_operator = parser->current;
            curr_prec     = precedence_lookup[curr_operator.type];
        }
    }
    return left;
};
/*====================================================== */

void print_indent(u32 indent_level)
{
    i32 i;
    for (i = 0; i < indent_level; ++i) {
        printf("  ");
    }
    printf("|-");
}
/*====================================================== */

void pretty_print_expression_node(const ExpressionNode *node, int indent_level)
{
    if (node == NULL) {
        print_indent(indent_level);
        printf("[NULL Node]\n");
        return;
    }

    print_indent(indent_level);

    switch (node->type) {
        case NodeType_Number:
            printf("NUMBER: %f\n", node->number);
            break;

        case NodeType_Negative:
            printf("NEGATIVE\n");
            break;

        case NodeType_Positive:
            printf("POSITIVE\n");
            break;

        case NodeType_Add:
            printf("BINARY ADD\n");
            pretty_print_expression_node(node->binary.right, indent_level + 1);
            pretty_print_expression_node(node->binary.left, indent_level + 1);
            break;

        case NodeType_Sub:
            printf("BINARY SUBTRACT\n");
            pretty_print_expression_node(node->binary.right, indent_level + 1);
            pretty_print_expression_node(node->binary.left, indent_level + 1);
            break;

        case NodeType_Mul:
            printf("BINARY MULTIPLY\n");
            pretty_print_expression_node(node->binary.right, indent_level + 1);
            pretty_print_expression_node(node->binary.left, indent_level + 1);
            break;

        case NodeType_Div:
            printf("BINARY DIVIDE\n");
            pretty_print_expression_node(node->binary.right, indent_level + 1);
            pretty_print_expression_node(node->binary.left, indent_level + 1);
            break;

        case NodeType_Pow:
            printf("BINARY POWER\n");
            pretty_print_expression_node(node->binary.right, indent_level + 1);
            pretty_print_expression_node(node->binary.left, indent_level + 1);
            break;

        default:
            printf("UNKNOWN NODE TYPE\n");
            break;
    }
}
/*====================================================== */

void tokenize(Token *token_list, const String str)
{
    i32 lenght    = str.len;
    i32 token_idx = 0;

    i32 i;
    for (i = 0; i < lenght; ++i) {
        char current_char = str.chars[i];
        if (current_char >= '0' && current_char <= '9') {
            i32 token_len         = get_token_number(&str, i);
            Token token           = {Token_Number, i};
            token_list[token_idx] = token;
            ++token_idx;
            i += token_len - 1;

            /*printf("NumberToken with lenght: %d\n", token_len);*/

        } else if ('*' == current_char) {
            Token token           = {Token_Star, i};
            token_list[token_idx] = token;
            /*printf("MultipyToken\n");*/
            ++token_idx;

        } else if ('(' == current_char) {
            Token token           = {Token_ParensOpen, i};
            token_list[token_idx] = token;
            /*printf("ParensOpenToken\n");*/
            ++token_idx;

        } else if (')' == current_char) {
            Token token           = {Token_ParensClose, i};
            token_list[token_idx] = token;
            /*printf("ParensCloseToken\n");*/
            ++token_idx;

        } else if ('/' == current_char) {
            Token token           = {Token_Slash, i};
            token_list[token_idx] = token;
            /*printf("DivideToken\n");*/
            ++token_idx;

        } else if ('+' == current_char) {
            Token token           = {Token_Plus, i};
            token_list[token_idx] = token;
            /*printf("PlusToken\n");*/
            ++token_idx;

        } else if ('-' == current_char) {
            Token token           = {Token_Minus, i};
            token_list[token_idx] = token;
            /*printf("MinusToken\n");*/
            ++token_idx;

        } else if ('^' == current_char) {
            Token token           = {Token_Caret, i};
            token_list[token_idx] = token;
            /*printf("CaretToken\n");*/
            ++token_idx;

        } else if (' ' == current_char) {
            continue;
        } else {
            printf(
                "\x1b[31mInvalid token '%c' at position %d.\x1b[0m\n",
                current_char,
                i + 1);
            exit(0);
        }
    }
    return;
}
/*====================================================== */

i32 get_token_number(const String *str, i32 offset)
{
    i32 i      = offset;
    i32 lenght = 0;
    do {
        if (!((str->chars[i] >= '0' && str->chars[i] <= '9') ||
              (str->chars[i] == '.'))) {
            break;
        }
        ++i;
        ++lenght;

    } while (i < str->len);

    return lenght;
}
/*====================================================== */

String StringNew(char *chars)
{
    i32 i  = 0;
    char c = chars[i];
    while ('\0' != c) {
        c = chars[++i];
    }
    String str = {};
    str.chars  = chars;
    str.len    = i;
    return str;
}
/*====================================================== */

int StringCompare(String str1, String str2)
{
    if (!(str1.len == str2.len)) {
        return 0;
    }
    u32 i;
    for (i = 0; i < str1.len; ++i) {
        if (!(str1.chars[i] == str2.chars[i])) {
            return 0;
        }
    }
    return 1;
}
