fn int sum(var int a, var int b)
{
    return a + b;
}

fn int sum(var int a, var int b, var int c)
{
    return a + b + c;
}

fn int main()
{
    return sum(28, 100) + sum(20,50,30);
}