#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * TODO:
 * - Write token calc function for numbers (ints first)
 * - Write Tokens to AST
 * - Write eval function
 */

#define GLOBAL_VARIABLE static;
typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t i32;

enum Tag {
    multiply,
    divide,
    plus,
    minus,
    integer,
};

enum TokenType {
    NumberToken  = 1,
    MultipyToken = 3,
    DivideToken  = 5,
    PlusToken    = 7,
    MinusToken   = 11,
    InvalidToken = -1

};

typedef struct string {
    char *chars;
    int len;
} String;

typedef struct token {
    enum TokenType type;
    u32 start;
} Token;

typedef struct ast_node {
    enum Tag tag;
    u32 main_token;
    struct {
        u32 lhs;
        u32 rhs;
    } data;

} AstNode;

String StringFromChars(char *chars);
i32 get_token_number(const String *str, i32 offset);
void tokenize(Token *token_list, const String str);
i32 evaluate(Token *token_list, f64 *value);
void push_arg(Token token);
void push_op(Token token);
Token pop_arg();
Token pop_op();

GLOBAL_VARIABLE Token operator_stack[256];
GLOBAL_VARIABLE f64 arg_stack[256];
GLOBAL_VARIABLE u32 arg_sptr = 0, op_sptr = 0;
GLOBAL_VARIABLE i32 state = 0; /* 0 = Awaiting expression
                                  1 = Awaiting operator
                               */

i32 main()
{
    char *string = "120 - 200 * 123 + 203 / 4322000";
    printf("test calc\n---------\n");
    printf("%s\n", string);
    String str = StringFromChars(string);
    printf("String %s, len: %d\n", str.chars, str.len);
    Token *token_list = (Token *)malloc(10 * sizeof(Token));
    tokenize(token_list, str);

    i32 i;
    for (i = 0; i < 10; ++i) {
        printf("%u\t%u\n", token_list[i].type, token_list[i].start);
    }

    f64 result = 0.0;
    evaluate(token_list, &result);
    printf("result: %.2e\n", result);

    free(token_list);
    return 0;
}

i32 evaluate(Token *token_list, f64 *value)
{
    i32 index;
    for (index = 0; token_list[index].type != 0; ++index) {
        switch (state) {
            case 0: /* awaiting operator */
                push_arg(token_list[index]);
            case 1: /* awaiting operator */
                break;
        }
    }
    return 0;
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
            Token token           = {NumberToken, i};
            token_list[token_idx] = token;
            ++token_idx;
            i += token_len;

            printf("NumberToken with lenght: %d\n", token_len);

        } else if ('*' == current_char) {
            Token token           = {MultipyToken, i};
            token_list[token_idx] = token;
            printf("MultipyToken\n");
            ++token_idx;

        } else if ('/' == current_char) {
            Token token           = {DivideToken, i};
            token_list[token_idx] = token;
            printf("DivideToken\n");
            ++token_idx;

        } else if ('+' == current_char) {
            Token token           = {PlusToken, i};
            token_list[token_idx] = token;
            printf("PlusToken\n");
            ++token_idx;

        } else if ('-' == current_char) {
            Token token           = {MinusToken, i};
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
        if (!(str->chars[i] >= '0' && str->chars[i] <= '9')) {
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

void push_arg(Token token)
{
    arg_stack[arg_sptr++] = token;
}
void push_op(Token token);
Token pop_arg();
Token pop_op();
