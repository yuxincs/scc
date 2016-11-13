
int fibonacci(int n)
{
    if(n == 1)
        return 1;
    if(n == 2)
        return 1;

    int x = fibonacci(n - 1);
    int y = fibonacci(n - 2);

    return x + y;
}

void main()
{
    int n = readint();
    writeint(fibonacci(n));
    return;
}

