float aa;

int func(int in){

    int a, b, c;
    a = 0;
    b = 1;
    c = -b;
    c = !b;
    switch (in)
    {
    case 1:
        a++;
        return a;
    case 2:
        b = a + c;
        return b;
    case 3:
        return c;
    default:
        return a + b + c;
    }

    return -1;
}

void func(float a){
    return;
}

const int N = 10;

int main()
{
    int a;
    //const int b[N + 2 * 4 - 99 / 99];
    int b;
    //int c[2.1];
    int min;
    a = 1 + 2 + 3;
    //b = 2 + 3 + 4;
    //c[0.1] = 1;

    while(1){
        break;
    }

    //int aa = 1 + c;
    if (1)
        min = a;
    else
        min = b;
    return min;
}
