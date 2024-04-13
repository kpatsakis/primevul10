levelSize (int min, int max, int l, LevelRoundingMode rmode)
{
    if (l < 0)
	throw IEX_NAMESPACE::ArgExc ("Argument not in valid range.");

    int a = max - min + 1;
    int b = (1 << l);
    int size = a / b;

    if (rmode == ROUND_UP && size * b < a)
	size += 1;

    return std::max (size, 1);
}