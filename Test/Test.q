#include<Test.hed>

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
    print(calculate(20,20,'/'));
    return 0;
}