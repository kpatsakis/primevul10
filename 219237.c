static InlineX int pad4(int value)
{
    int remainder = value & 3;
    if (!remainder) return value;
    return value + 4 - remainder;
}