int x;
float f;
x = 3;
f = x;          /* int widens to float */
f = f * 2.5 + x;
{
    int x;      /* shadows the outer x: allowed in a nested scope */
    x = 7;
    print x;
}
print f;
