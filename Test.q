fn int calculate(var int a, var int b,var char operator)
{
    if(operator == '+')
    {
        return a + b;
    }
    if(operator == '-')
    {
        return a - b;
    }
    if(operator == '*')
    {
        return a * b;
    }
    if(operator == '/')
    {
        return a / b;
    }
    if(operator == '%')
    {
        return a % b;
    }
    return 0;
}

fn int main()
{
    return calculate(20,20,'/');
}