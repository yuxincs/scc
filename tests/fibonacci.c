int main()
{
    int a = 0, b = 1, i = 0, n;
    n = readint();
    while(i < n)
    {
        int c = a + b;
        writeint(b);
        a = b;
        b = c;
        i = i + 1;
    }
    return 0;
}

