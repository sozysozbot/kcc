#include <stdio.h>
#include <string.h>

int ASSERT(int expected, int actual, char *name)
{
    if (expected == actual)
        return 0;

    printf("name:<%s> failed!!\n", name);
    printf("expected %d -> actual %d\n", expected, actual);
    exit(1);
}

// ローカル変数
int local1()
{
    int a;
    a = 6;
    return a;
}
int local2()
{
    int a;
    int b;
    a = 2;
    b = a;
    return b;
}
int local3()
{
    int a;
    int b;
    int c;
    a = 1;
    b = 2;
    c = a + b;
    return c;
}
int local4()
{
    int value;
    value = 12;
    return value;
}
int local5()
{
    int val;
    int t;
    val = 1;
    t = 3 * (val + 1);
    return t;
}
int local6()
{
    int num;
    int test;
    num = 1;
    test = (num * 10) / 2;
    return test;
}
int local7()
{
    int a;
    a = 1;
    a = a + 3;
    return a;
}
int local8()
{
    int a2;
    int A_1;
    a2 = 1;
    A_1 = 5;
    return a2 * A_1;
}
// 代入演算子
int assign1()
{
    int a;
    a = 1;
    a += 1;
    return a;
}
int assign2()
{
    int a;
    a = 10;
    a -= 6;
    return a;
}
int assign3()
{
    int a;
    a = 2;
    a *= 3;
    return a;
}
int assign4()
{
    int a;
    a = 20;
    a /= 2;
    return a;
}
int assign5()
{
    int a;
    int b;
    a = 5;
    b = a += 5;
    return b;
}
int assign6()
{
    int a;
    int b;
    a = 0;
    b = (a = a + 1) + (a = a + 1);
    return b + a;
}
int assign7()
{
    int a;
    int b;
    a = 0;
    b = (a = a - 1) + (a = a - 1);
    return b * a;
}
// if else
int if_else1()
{
    if (1)
        return 4;
}
int if_else2()
{
    if (2 + 3 == 5)
        return 3 * 2;
}
int if_else3()
{
    int a;
    a = 2 * 3;
    if (a == 6)
        return a + 4;
    else
        return 0;
}
int if_else4()
{
    int a;
    a = 2 * 3;
    if (a < 5)
        return a + 4;
    else
        return 0;
}
int if_else5()
{
    int a;
    int n;
    a = 1;
    n = 0;
    if (a == 1)
    {
        n = 1;
    }
    if (a == 2)
    {
        n = 2;
    }
    return n;
}
int if_else6()
{
    int sum;
    sum = 1;
    if (sum % 2 != 0)
    {
        sum += 1;
        if (sum % 2 == 0)
        {
            sum += 1;
        }
        else
        {
            sum = 0;
        }
    }
    else
    {
        sum = 0;
    }
    return sum;
}
int if_else7()
{
    int a;
    a = 10;
    if (a == 10)
    {
        a = 20;
    }
    if (a == 20)
    {
        a = 30;
    }
    return a;
}

// while
int while1()
{
    int i;
    i = 0;
    while (i < 10)
        i += 1;
    return i;
}
int while2()
{
    int i;
    i = 0;
    int j;
    while (i < 10)
    {
        j = 0;
        while (j < 3)
        {
            i += 1;
            j += 1;
        }
    }
    return i;
}
int while3()
{
    int i;
    int sum;
    sum = 0;
    i = 0;
    while (i < 5)
    {
        sum += 1;
        i += 1;
    }
    while (i < 10)
    {
        sum += 1;
        i += 1;
    }
    return sum;
}

// for
int for1()
{
    int a;
    int i;
    a = 0;
    for (i = 0; i < 10; i += 1)
        a = a + 1;
    return a;
}
int for2()
{
    int i;
    int sum;
    sum = 0;
    for (i = 0; i < 3; i += 1)
    {
        sum += 1;
    }
    for (i = 0; i < 3; i += 1)
    {
        sum += 1;
    }
    return sum;
}
int for3()
{
    int sum;
    int i;
    int j;
    sum = 0;
    for (i = 0; i < 3; i += 1)
    {
        for (j = 0; j < 3; j += 1)
        {
            sum += 1;
        }
    }
    return sum;
}

// for-while
int for_while1()
{
    int i;
    int sum;
    sum = 0;
    for (i = 0; i < 3; i += 1)
    {
        sum += 1;
    }
    while (i < 5)
    {
        sum += 1;
        i += 1;
    }
    return sum;
}

// block
int block1() { return 1; }
int block2()
{
    int a;
    a = 1;
    if (a / 2 == 0)
    {
        a *= 2;
        return a;
    }
    else
    {
        return a;
    }
}
int block3()
{
    int a;
    int i;
    a = 0;
    for (i = 1; i <= 5; i += 1)
    {
        a += i;
        a += 1;
    }
    return a;
}

// null statement
int null_statement1()
{
    ;
    ;
    ;
    ;
    ;
    ;
    return 1;
}

// func call (関数の実態はリンクする)
int func_call1() { return ret(); }
int func_call2() { return ret(); }
int func_call3()
{
    int a;
    a = 1;
    return ret() + a * 2;
}
int func_call4() { return constant(4); }
int func_call5() { return add(1, 2); }
int func_call6() { return add6(1, 2, 3, 4, 5, 6); }
int func_call7() { return add6(1, 2, add6(3, 4, 5, 6, 7, 8), 9, 10, 11); }
int func_call8() { return add6(1, 2, add6(3, add6(4, 5, 6, 7, 8, 9), 10, 11, 12, 13), 14, 15, 16); }

// func define
int func_define1_test() { return 2; }
int func_define1() { return func_define1_test(); }
int func_define2_test() { return 4; }
int func_define2()
{
    int a;
    a = 1;
    return a + func_define2_test();
}
int func_define3_test() { return 1; }
int func_define3() { return func_define3_test() + func_define3_test(); }

int func_define4_test(int a) { return a; }
int func_define4() { return func_define4_test(6); }
int func_define5_add2(int a, int b) { return a + b; }
int func_define5() { return func_define5_add2(1, 2); }
int func_define6_test(int a, int b)
{
    int c;
    c = 10;
    return c + a + b;
}
int func_define6() { return func_define6_test(2, 3); }
int func_define7_sum(int n)
{
    if (n == 1)
    {
        return 1;
    }
    return func_define7_sum(n - 1) + func_define7_sum(n - 1);
}
int func_define7() { return func_define7_sum(4); }
int func_define8_fib(int n)
{
    if (n == 1)
    {
        return 1;
    }
    if (n == 2)
    {
        return 1;
    }
    return func_define8_fib(n - 1) + func_define8_fib(n - 2);
}
int func_define8() { return func_define8_fib(10); }

// ポインター
int pointer1()
{
    int x;
    int *y;
    y = &x;
    *y = 3;
    return x;
}
int pointer2()
{
    int a;
    int *b;
    int **c;
    int ***d;
    d = &c;
    c = &b;
    b = &a;
    a = 10;
    return ***d;
}
int pointer3()
{
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 1;
    return *q;
}
int pointer4()
{
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 3;
    q = q - 1;
    return *q;
}
int pointer5_ptr(int *a)
{
    *a = 10;
    return 0;
}
int pointer5()
{
    int b;
    b = 1;
    pointer5_ptr(&b);
    return b;
}

// sizeof
int sizeof1()
{
    int x;
    return sizeof x;
}
int sizeof2()
{
    int x;
    return sizeof(x);
}
int sizeof3()
{
    int x;
    int *y;
    return sizeof(x) + sizeof(y);
}
int sizeof4()
{
    int *x;
    return sizeof(*x);
}
int sizeof5()
{
    int a[5];
    return sizeof(a);
}

// 配列
int array1()
{
    int a[2];
    *a = 1;
    *(a + 1) = 2;
    int *p;
    p = a;
    return *p + *(p + 1);
}
int array2()
{
    int a[3];
    *(a) = 1;
    *(a + 1) = 2;
    *(a + 2) = 3;
    int i;
    int sum;
    for (i = 0; i < 3; i += 1)
    {
        sum += *(a + i);
    }
    return sum;
}
int array3()
{
    int a;
    int b[2];
    a = 1;
    b[1] = a;
    return b[1] * 2;
}
int array4()
{
    int a[2];
    int b[2];
    int c;
    c = 1;
    a[0] = c;
    a[1] = a[0] + 1;
    b[0] = a[1] + 1;
    b[1] = b[0] + 1;
    return b[1];
}
int array5()
{
    int a[3][3];
    int i;
    int j;
    for (i = 0; i < 3; i += 1)
    {
        for (j = 0; j < 3; j += 1)
        {
            a[i][j] = 2 * i + j;
        }
    }
    return a[2][1];
}

// MOD
int mod1_gcd(int a, int b)
{
    if (b == 0)
    {
        return a;
    }
    return mod1_gcd(b, a % b);
}
int mod1() { return mod1_gcd(630, 300); }
int mod2()
{
    int a;
    int b;
    int c;
    a = 100;
    b = 21;
    c = 3;
    return a % b % c;
}

int char1()
{
    char x[3];
    x[0] = -1;
    x[1] = 2;
    int y;
    y = 4;
    return x[0] + y;
}

// string_literal
int string_literal1()
{
    char *a;
    a = "Hello, C-compiler\n";
    printf("%s\n", a);
    return 0;
}

// %=
int assign_mod1()
{
    int a;
    a = 10;
    a %= 4;
    return a;
}

// preinc
int preinc1()
{
    int a;
    int b;
    a = 0;
    b = (++a) + (++a);
    return b + a;
}
int preinc2()
{
    int i;
    int sum;
    sum = 0;
    for (i = 0; i < 10; ++i)
    {
        sum += i;
    }
    return sum;
}

// postinc
int postinc1() {
    int a; int b;
    a = 0;
    b = 0;
    a = b++;
    return a;
}
int postinc2() {
    int i;
    int sum;
    sum = 0;
    for (i=0;i<10;i++) {
        sum += i;
    }
    return sum;
}

// predec
int predec1()
{
    int a;
    int b;
    a = 0;
    b = (--a) + (--a);
    return b * a;
}
int predec2()
{
    int i;
    int sum;
    sum = 0;
    for (i = 9; i >= 0; --i)
    {
        sum += i;
    }
    return sum;
}

// postdec
int postdec1() {
    int a; int b;
    a = 0;
    b = 0;
    a = b--;
    return a;
}
int postdec2() {
    int i;
    int sum;
    sum = 0;
    for (i=9;i>=0;i--) {
        sum += i;
    }
    return sum;
}

// assign_initializer
int assign_initializer1() {
    int sum = 0;
    for (int i=0;i<10;i++) {
        sum += i;
    }
    return sum;
}

int assign_initializer2() {
    int height = 10;
    int width = 20;
    int res = height * width / 2;
    return res;
}

int assign_initializer3() {
    char *s1 = "Hello";
    char *s2 = "Hello";
    int is_same = (strcmp(s1, s2) == 0);
    return is_same;
}

int main()
{
    ASSERT(6, local1(), "local1");
    ASSERT(2, local2(), "local2");
    ASSERT(3, local3(), "local3");
    ASSERT(12, local4(), "local4");
    ASSERT(6, local5(), "local5");
    ASSERT(5, local6(), "local6");
    ASSERT(4, local7(), "local7");
    ASSERT(5, local8(), "local8");

    ASSERT(2, assign1(), "assign1");
    ASSERT(4, assign2(), "assign2");
    ASSERT(6, assign3(), "assign3");
    ASSERT(10, assign4(), "assign4");
    ASSERT(10, assign5(), "assign5");
    ASSERT(5, assign6(), "assign6");
    ASSERT(6, assign7(), "assign7");

    ASSERT(4, if_else1(), "if_else1");
    ASSERT(6, if_else2(), "if_else2");
    ASSERT(10, if_else3(), "if_else3");
    ASSERT(0, if_else4(), "if_else4");
    ASSERT(1, if_else5(), "if_else5");
    ASSERT(3, if_else6(), "if_else6");
    ASSERT(30, if_else7(), "if_else7");

    ASSERT(10, while1(), "while1");
    ASSERT(12, while2(), "while2");
    ASSERT(10, while3(), "while3");

    ASSERT(10, for1(), "for1");
    ASSERT(6, for2(), "for2");
    ASSERT(9, for3(), "for3");

    ASSERT(5, for_while1(), "for_while1");

    ASSERT(1, block1(), "bolck1");
    ASSERT(2, block2(), "bolck2");
    ASSERT(20, block3(), "bolck3");

    ASSERT(1, null_statement1(), "null_statement1");

    ASSERT(3, func_call1(), "func_call1");
    ASSERT(3, func_call2(), "func_call2");
    ASSERT(5, func_call3(), "func_call3");
    ASSERT(4, func_call4(), "func_call4");
    ASSERT(3, func_call5(), "func_call5");
    ASSERT(21, func_call6(), "func_call6");
    ASSERT(66, func_call7(), "func_call7");
    ASSERT(136, func_call8(), "func_call8");

    ASSERT(2, func_define1(), "func_define1");
    ASSERT(5, func_define2(), "func_define2");
    ASSERT(2, func_define3(), "func_define3");
    ASSERT(6, func_define4(), "func_define4");
    ASSERT(3, func_define5(), "func_define5");
    ASSERT(15, func_define6(), "func_define6");
    ASSERT(8, func_define7(), "func_define7");
    ASSERT(55, func_define8(), "func_define8");

    ASSERT(3, pointer1(), "pointer1");
    ASSERT(10, pointer2(), "pointer2");
    ASSERT(2, pointer3(), "pointer3");
    ASSERT(4, pointer4(), "pointer4");
    ASSERT(10, pointer5(), "pointer5");

    ASSERT(4, sizeof1(), "sizeof1");
    ASSERT(4, sizeof2(), "sizeof2");
    ASSERT(12, sizeof3(), "sizeof3");
    ASSERT(4, sizeof4(), "sizeof4");
    ASSERT(20, sizeof5(), "sizeof5");

    ASSERT(3, array1(), "array1");
    ASSERT(6, array2(), "array2");
    ASSERT(2, array3(), "array3");
    ASSERT(4, array4(), "array4");
    ASSERT(5, array5(), "array5");

    ASSERT(30, mod1(), "mod1");
    ASSERT(1, mod2(), "mod2");

    ASSERT(3, char1(), "char1");

    ASSERT(0, string_literal1(), "string_literal");

    ASSERT(2, assign_mod1(), "assign_mod1");

    ASSERT(5, preinc1(), "preinc1");
    ASSERT(45, preinc2(), "preinc2");
    ASSERT(6, predec1(), "predec1");
    ASSERT(45, predec2(), "predec2");

    ASSERT(0, postinc1(), "postinc1");
    ASSERT(45, postinc2(), "postinc2");
    ASSERT(0, postdec1(), "postdec1");
    ASSERT(45, postdec2(), "postdec2");

    ASSERT(45, assign_initializer1(), "assign_initializer1");
    ASSERT(100, assign_initializer2(), "assign_initializer2");
    ASSERT(1, assign_initializer3(), "assign_initializer3");

    printf("ALL TEST OF test.c SUCCESS :)\n");
    return 0;
}