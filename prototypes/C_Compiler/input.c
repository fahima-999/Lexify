#include <stdio.h>

// Lexify demo: declarations, arithmetic, if/else, while, do-while, for, switch, break/continue.
int main() {
    int i, sum = 0;

    // for loop + continue: sum of odd numbers up to 10
    for (i = 1; i <= 10; i++) {
        if (i % 2 == 0) continue;
        sum += i;
    }
    printf("sum of odd numbers = %d\n", sum);

    // while loop: factorial of 5
    int n = 5;
    long fact = 1;
    while (n > 1) {
        fact *= n;
        n--;
    }
    printf("5! = %ld\n", fact);

    // do-while: digit sum of 12345
    int num = 12345, digits = 0;
    do {
        digits += num % 10;
        num /= 10;
    } while (num > 0);
    printf("digit sum = %d\n", digits);

    // if / else if / else
    int marks = 72;
    if (marks >= 80) printf("grade A\n");
    else if (marks >= 60) printf("grade B\n");
    else printf("grade C\n");

    // switch
    int day = 3;
    switch (day) {
        case 1: printf("Mon\n"); break;
        case 3: printf("Wed\n"); break;
        default: printf("other\n");
    }

    // float arithmetic and casting
    int a = 7, b = 2;
    float avg = (float)(a + b) / 2;
    printf("avg = %.2f, int division = %d\n", avg, a / b);

    // nested loops: small triangle
    for (int r = 1; r <= 4; r++) {
        for (int c = 1; c <= r; c++) printf("*");
        printf("\n");
    }
    return 0;
}
