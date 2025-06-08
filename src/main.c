#include "calc.c"

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
