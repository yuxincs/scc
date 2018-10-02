int main()
{
    int a = 0, b = 1, i = 1, n;
    n = __READINT();
    while(i < n)
    {
        int c = a + b;
        a = b;
        b = c;
        i = i + 1;
    }
    __WRITEINT(b);
    return 0;
}

