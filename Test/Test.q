#include<Test.hed>.print

fn int main()
{
    var int count = 0;
    for (var int i = 1;i<1000;i = i + 1)
    {
        for (var int j = 1;j<1000;j = j + 1)
        {
            for (var int k = 1;k<1000;k = k + 1)
            {
                if(i * i + j * j == k * k && i < j)
                {
                    count = count + 1;
                }
            }
        }
    }
    print(count);

    return 0;
}