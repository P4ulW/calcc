#include <stdint.h>
#include <stdio.h>

typedef struct string {
  char *chars;
  int len;
} string_t;

typedef struct token {
  uint8_t type;
  char *start;
} token_t;

string_t StringFromChars(char *chars) {
  int i = 0;
  char c = chars[i];
  while ('\0' != c) {
    c = chars[++i];
  }
  string_t str = {};
  str.chars = chars;
  str.len = i;
  return str;
}

/*
 * TODO:
 * - Write token functions for each typ
 * - Write Tokens to AST
 * - Write eval function
 */

void Tokenize(token_t *token_list, const string_t str) {
  int lenght = str.len;
  int token_idx = 0;

  for (int i = 0; i < lenght; ++i) {
    char current_char = str.chars[i];
    if (current_char > '0' || current_char < '9') {
      GetTokenNumber();
      token_idx += 1
    } else if ('*' == current_char) {
      GetTokenMultiply();
      token_idx += 1
    } else if ('\\' == current_char) {
      GetTokenDivide();
      token_idx += 1
    } else if ('+' == current_char) {
      GetTokenPlus();
      token_idx += 1
    } else if ('-' == current_char) {
      GetTokenMinus();
      token_idx += 1
    }
  }
  return token_list;
}

int main() {
  char *string = "1 + 2";
  printf("test calc\n---------\n");
  printf("%s\n", string);
  string_t str = StringFromChars(string);
  printf("String string %s, len: %d", str.chars, str.len);

  return 0;
}
