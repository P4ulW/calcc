#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vec.h"

typedef struct string {
    char *chars;
    int len;
} string_t;

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
} token_t;

string_t StringFromChars(char *chars)
{
    int i  = 0;
    char c = chars[i];
    while ('\0' != c) {
        c = chars[++i];
    }
    string_t str = {};
    str.chars    = chars;
    str.len      = i;
    return str;
}

typedef struct ast_node {
};

/*
 * TODO:
 * - Write token calc function for numbers (ints first)
 * - Write Tokens to AST
 * - Write eval function
 */

int GetTokenNumber(const string_t *str, int offset)
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

void Tokenize(token_t *token_list, const string_t str)
{
    int lenght    = str.len;
    int token_idx = 0;

    for (int i = 0; i < lenght; ++i) {
        char current_char = str.chars[i];
        if (current_char >= '0' && current_char <= '9') {
            int token_len         = GetTokenNumber(&str, i);
            token_t token         = {NumberToken, i};
            token_list[token_idx] = token;
            ++token_idx;
            i += token_len;

            printf("Number Token with lenght: %d\n", token_len);

        } else if ('*' == current_char) {
            token_t token         = {MultipyToken, i};
            token_list[token_idx] = token;
            ++token_idx;

        } else if ('\\' == current_char) {
            token_t token         = {DivideToken, i};
            token_list[token_idx] = token;
            ++token_idx;

        } else if ('+' == current_char) {
            token_t token         = {PlusToken, i};
            token_list[token_idx] = token;
            ++token_idx;

        } else if ('-' == current_char) {
            token_t token         = {MinusToken, i};
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
    string_t str = StringFromChars(string);
    printf("String string %s, len: %d\n", str.chars, str.len);
    token_t *token_list = (token_t *)malloc(10 * sizeof(token_t));
    Tokenize(token_list, str);

    for (int i = 0; i < 1000; ++i) {
        printf("%u\t%u\n", token_list[i].type, token_list[i].start);
    }

    free(token_list);
    return 0;
}
