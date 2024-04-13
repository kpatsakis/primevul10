Jsi_Number Jsi_NumberInfinity(int i)
{
    Jsi_Number r = INFINITY;
    if (i < 0) r = -r;
    return r;
}