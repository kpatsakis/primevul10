jsi_HashArray(const void *key, size_t length )
{
    const uchar *string = (const uchar *) key;
    unsigned int result = 0;
    int i;

     for (i=0 ; i<(int)length; i++) {
        result += (result<<3) + string[i];
    }
    return (jsi_Hash)result;
}