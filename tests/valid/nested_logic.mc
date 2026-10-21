// nested blocks, float, relational + logical operators
int a;
float f;
bool ok;
a = 7 % 3;
f = 3.14;
/* block comment */
ok = (a >= 1) && !(f < 1.0) || false;
if (ok) {
    int inner;
    inner = a * 2 + 1;
    while (inner != 0) {
        inner = inner - 1;
        if (inner == 2) print inner;
    }
}
