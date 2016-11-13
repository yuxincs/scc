int fact(int n)
{
    if (n == 1)
        return n;
    
    return (n * fact(n - 1));
}

int main()
{
    int m, result;
    m = readint();
    result = 1;
    if(m > 1)
        result = fact(m);

    writeint(result);
    return 0;
}