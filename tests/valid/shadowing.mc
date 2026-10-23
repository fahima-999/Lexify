// inner x shadows outer x; TAC must keep them apart (x vs x_1)
int x;
x = 1;
if (x == 1) {
    int x;
    x = 2;
    print x;
}
print x;
