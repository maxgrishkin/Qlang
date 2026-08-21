#include<Test.hed>

fn int fib(var int index)
{
    if(index <= 1){
        return 1;
    }
    return fib(index - 1) * index;
}

fn int main()
{
    for(var int i = 1; i<=20; i = i+1){
        print(fib(i));
    }

    return 0;
}