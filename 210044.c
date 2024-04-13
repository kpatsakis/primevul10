HashString( const char *string)
{
    jsi_Hash result = 0;
    jsi_Hash c;
    while ((c = *string++) != 0)
        result += (result << 3) + c;

    return (jsi_Hash)result;
}