#include "arena.h"
#include "calc.c"
#include <math.h>
#include <sys/time.h>
#include <time.h>

f64 eval(String expr)
{
    Arena arena;
    arena_init(&arena, 4096);
    Token *tokenlist = arena_alloc(&arena, expr.len * sizeof(Token));

    tokenize(tokenlist, expr);

    Parser parser = {
        .tokenlist      = tokenlist,
        .current        = tokenlist[0],
        .curr_token_idx = 0,
        .str            = expr};

    ExpressionNode *node_root =
        parser_parse_expression(&arena, &parser, Precedence_Min);
    f64 result = evaluate(node_root);
    arena_free(&arena);
    return result;
}

#define RUN_TEST(expression_str, expected_value)                               \
    total_tests++;                                                             \
    str      = StringNew(expression_str);                                      \
    result   = eval(str);                                                      \
    expected = (expected_value); /* Cast to double for consistency with f64 */ \
    if (fabs(result - expected) < 1e-9) { /* Compare floats with tolerance */  \
        printf("\033[32mTest passed: %s = %f\033[0m\n", str.chars, result);    \
        tests_passed++;                                                        \
    } else {                                                                   \
        printf(                                                                \
            "\033[31mTest failed: Wrong result for %s\033[0m\n", str.chars);   \
        printf("  Expected: %f, Got: %f\n", expected, result);                 \
    }                                                                          \
    printf("\n");

int main()
{

    clock_t start_time = clock();

    f64 result;
    f64 expected;
    int tests_passed = 0;
    int total_tests  = 0;
    String str;

    RUN_TEST("1+1", 1.0 + 1.0);
    RUN_TEST("1*5+(3-2)/2", 1.0 * 5.0 + (3.0 - 2.0) / 2.0);
    RUN_TEST("1^5+(3-2)/2^12.4", pow(1.0, 5.0) + (3.0 - 2.0) / pow(2.0, 12.4));

    /* 1. Basic operations with different numbers and order*/
    RUN_TEST("10-4", 10.0 - 4.0);
    RUN_TEST("7*8", 7.0 * 8.0);
    RUN_TEST("20/5", 20.0 / 5.0);
    RUN_TEST("1.5+2.5", 1.5 + 2.5);
    RUN_TEST("100.0/4.0", 100.0 / 4.0);

    /*2. Operator Precedence*/
    RUN_TEST("2+3*4", 2.0 + 3.0 * 4.0);
    RUN_TEST("10-4/2", 10.0 - 4.0 / 2.0);
    RUN_TEST("5*2+8/4", 5.0 * 2.0 + 8.0 / 4.0);

    /*3. Parentheses*/
    RUN_TEST("(2+3)*4", (2.0 + 3.0) * 4.0);
    RUN_TEST("10/(5-3)", 10.0 / (5.0 - 3.0));
    RUN_TEST("(1+2)*(3+4)", (1.0 + 2.0) * (3.0 + 4.0));
    RUN_TEST("((10+2)/3)-1", ((10.0 + 2.0) / 3.0) - 1.0);

    /*4. Negative Numbers / Unary Minus*/
    RUN_TEST("-5+3", -5.0 + 3.0);
    RUN_TEST("10--2", 10.0 - (-2.0));
    RUN_TEST("10-(-2)", 10.0 - (-2.0));
    RUN_TEST("5*(-3)", 5.0 * (-3.0));
    RUN_TEST("-(2+3)", -(2.0 + 3.0));
    RUN_TEST("-10/-2", -10.0 / -2.0);
    RUN_TEST("1+-2", 1.0 + -2.0);

    /*5. Floating-point numbers*/
    RUN_TEST("0.1+0.2", 0.1 + 0.2);
    RUN_TEST("1.23*4.56", 1.23 * 4.56);
    RUN_TEST("10.0/3.0", 10.0 / 3.0);
    RUN_TEST("(-0.5)*(-0.5)", (-0.5) * (-0.5));

    /*6. Power operator (^)*/
    RUN_TEST("2^3", pow(2.0, 3.0));
    RUN_TEST("3^2+1", pow(3.0, 2.0) + 1.0);
    RUN_TEST("(2+1)^3", pow(2.0 + 1.0, 3.0));
    RUN_TEST("10^0.5", pow(10.0, 0.5));
    RUN_TEST("2^-3", pow(2.0, -3.0));

    /*7. Edge cases / More complex expressions*/
    RUN_TEST("5", 5.0);
    RUN_TEST("0", 0.0);
    /*RUN_TEST("1/0", 1.0/0.0);
    /*crash, check your eval) RUN_TEST("0/0", 0.0/0.0);
     * in*/
    /*NaN, check your eval)*/
    RUN_TEST("1000000000+1", 1000000000.0 + 1.0);
    RUN_TEST("((((1))))", 1.0);
    RUN_TEST("2*(3+(4*5-(6/2)))", 2.0 * (3.0 + (4.0 * 5.0 - (6.0 / 2.0))));

    printf("\n--- Test Summary ---\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", total_tests - tests_passed);

    clock_t end_time = clock();
    double cpu_time_used =
        ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000 * 1000;
    printf("CPU time taken: %f µs\n", cpu_time_used);
    return 0;
}
