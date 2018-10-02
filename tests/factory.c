int fact(int n)
{
    if (n == 1)
        return n;
    else
        return (n * fact(n - 1));
}

int main()
{
    int m, result;
    m = __READINT();
    if(m > 1)
        result = fact(m);
    else
        result = 1;
    __WRITEINT(result);
    return 0;
}