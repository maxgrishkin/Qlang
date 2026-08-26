fn int main()
{
    var int i = 1;
    label:
    i = i + 1;
    if(i < 228)
    {
        goto label;
    }
    return i;
}