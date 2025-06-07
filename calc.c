#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vec.h"

typedef struct string {
    char *chars;
    int len;
} String;

enum TokenType {
    NumberToken  = 1,
    MultipyToken = 3,
    DivideToken  = 5,
    PlusToken    = 7,
    MinusToken   = 11,
    InvalidToken = -1

};

typedef struct token {
    enum TokenType type;
    uint start;
} Token;

String StringFromChars(char *chars)
{
    int i  = 0;
    char c = chars[i];
    while ('\0' != c) {
        c = chars[++i];
    }
    String str = {};
    str.chars  = chars;
    str.len    = i;
    return str;
}

typedef struct ast_node {
} AstNode;

/*
 * TODO:
 * - Write token calc function for numbers (ints first)
 * - Write Tokens to AST
 * - Write eval function
 */

int get_token_number(const String *str, int offset)
{
    int i      = offset;
    int lenght = 0;
    do {
        if (!(str->chars[i] >= '0' && str->chars[i] <= '9')) {
            break;
        }
        ++i;
        ++lenght;

    } while (i < str->len);

    return lenght;
}

void tokenize(Token *token_list, const String str)
{
    int lenght    = str.len;
    int token_idx = 0;

    for (int i = 0; i < lenght; ++i) {
        char current_char = str.chars[i];
        if (current_char >= '0' && current_char <= '9') {
            int token_len         = get_token_number(&str, i);
            Token token           = {NumberToken, i};
            token_list[token_idx] = token;
            ++token_idx;
            i += token_len;

            printf("Number Token with lenght: %d\n", token_len);

        } else if ('*' == current_char) {
            Token token           = {MultipyToken, i};
            token_list[token_idx] = token;
            ++token_idx;

        } else if ('\\' == current_char) {
            Token token           = {DivideToken, i};
            token_list[token_idx] = token;
            ++token_idx;

        } else if ('+' == current_char) {
            Token token           = {PlusToken, i};
            token_list[token_idx] = token;
            ++token_idx;

        } else if ('-' == current_char) {
            Token token           = {MinusToken, i};
            token_list[token_idx] = token;
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

int main()
{
    char *string = "120 + 200 + 123 + 203 + 4322000";
    printf("test calc\n---------\n");
    printf("%s\n", string);
    String str = StringFromChars(string);
    printf("String string %s, len: %d\n", str.chars, str.len);
    Token *token_list = (Token *)malloc(10 * sizeof(Token));
    tokenize(token_list, str);

    for (int i = 0; i < 1000; ++i) {
        printf("%u\t%u\n", token_list[i].type, token_list[i].start);
    }

    free(token_list);
    return 0;
}
