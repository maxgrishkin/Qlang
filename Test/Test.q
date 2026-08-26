#include<Test.hed>

fn int main()
{
    for (var int i = 1;i < 1000;i = i + 1)
    {
        for (var int j = 1;j < 1000;j = j + 1)
        {
            for (var int k = 1;k < 1000;k = k + 1)
            {
                if (i < j)
                {
                    if (j < k)
                    {
                        if (i * i + j * j == k * k)
                        {
                            print(i);
                            print(j);
                            print(k);
                            print(0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}