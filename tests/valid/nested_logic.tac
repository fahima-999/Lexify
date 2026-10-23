    t1 = 7 % 3
    a = t1
    f = 3.14
    t2 = a >= 1
    t3 = f < 1.0
    t4 = ! t3
    t5 = t2 && t4
    t6 = t5 || false
    ok = t6
    ifFalse ok goto L1
    t7 = a * 2
    t8 = t7 + 1
    inner = t8
L2:
    t9 = inner != 0
    ifFalse t9 goto L3
    t10 = inner - 1
    inner = t10
    t11 = inner == 2
    ifFalse t11 goto L4
    print inner
L4:
    goto L2
L3:
L1:
