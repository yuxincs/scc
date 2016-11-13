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
    m = readint(); 
    if(m > 1)
        result = fact(m);
    else
        result = 1;
    writeint(result);
    return 0;
}