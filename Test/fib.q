fn int fib(var int a)
{
    if (a == 1)
    {
        return 1;
    }
    if (a == 2)
    {
        return 1;
    }
    return fib(a - 1) + fib(a - 2);
}