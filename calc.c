#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "arena.c"

/*
 * TODO:
 * - impl letter into tokenizer (for e.g. pi, e, etc)
 * - Write token calc function for numbers (ints first)
 * - Write Tokens to AST
 * - Write eval function
 */

#define GLOBAL_VARIABLE static;
typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t i32;

enum TokenType {
    Token_Error = -1,
    Token_Number,
    Token_Mul,
    Token_Div,
    Token_Add,
    Token_Sub,
};

typedef struct string {
    char *chars;
    int len;
} String;

typedef struct token {
    enum TokenType type;
    u32 start;
} Token;

typedef u32 ExpressionNodeKind;
enum {
    NodeType_Error,
    NodeType_Number,
    NodeType_Add,
    NodeType_Sub,
    NodeType_Mul,
    NodeType_Div
};

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

String StringFromChars(char *chars);
i32 get_token_number(const String *str, i32 offset);
void tokenize(Token *token_list, const String str);
f64 evaluate(ExpressionNode *node);
void push_arg(Token token);
void push_op(Token token);
f64 number_from_token(Token token, const String *str);
Token pop_arg();
Token pop_op();

i32 main()
{
    Arena arena;
    arena_init(&arena, 4096);
    char *string = "12.01 - 200 * 123 + 203 / 4322000";
    printf("test calc\n---------\n");
    printf("%s\n", string);
    String str = StringFromChars(string);
    printf("String %s, len: %d\n", str.chars, str.len);
    Token *token_list = (Token *)arena_alloc(&arena, 100 * sizeof(Token));
    tokenize(token_list, str);

    i32 i;
    for (i = 0; i < 10; ++i) {
        printf("%u\t%u\n", token_list[i].type, token_list[i].start);
    }

    ExpressionNode left_node  = {NodeType_Number, 1123.04032};
    ExpressionNode right_node = {NodeType_Number, 23.3};
    ExpressionNode top_node   = {
        NodeType_Mul, {.binary = {&left_node, &right_node}}};
    f64 result = evaluate(&top_node);

    printf("result: %.2e\n", result);

    arena_free(&arena);
    return 0;
}

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
    }
    return 0.0;
}

typedef struct Parser {
    Token *tokenlist;
    Token current;
    u32 curr_token_idx;
} Parser;

void parser_advance(Parser *parser)
{
    parser->curr_token_idx++;
    parser->current = parser->tokenlist[parser->curr_token_idx];
}

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

ExpressionNode *
parser_parse_number(Arena *arena, Parser *parser, const String *str)
{
    ExpressionNode *node = arena_alloc(arena, sizeof(ExpressionNode));
    node->type           = NodeType_Number;
    node->number         = number_from_token(parser->current, str);
    parser_advance(parser);
    return node;
}

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
            i += token_len;

            printf("NumberToken with lenght: %d\n", token_len);

        } else if ('*' == current_char) {
            Token token           = {Token_Mul, i};
            token_list[token_idx] = token;
            printf("MultipyToken\n");
            ++token_idx;

        } else if ('/' == current_char) {
            Token token           = {Token_Div, i};
            token_list[token_idx] = token;
            printf("DivideToken\n");
            ++token_idx;

        } else if ('+' == current_char) {
            Token token           = {Token_Add, i};
            token_list[token_idx] = token;
            printf("PlusToken\n");
            ++token_idx;

        } else if ('-' == current_char) {
            Token token           = {Token_Sub, i};
            token_list[token_idx] = token;
            printf("MinusToken\n");
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

String StringFromChars(char *chars)
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
