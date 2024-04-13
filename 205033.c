static int t_fromb64(unsigned char *a, const char *src)
{
    char *loc;
    int i, j;
    int size;

    while (*src && (*src == ' ' || *src == '\t' || *src == '\n'))
        ++src;
    size = strlen(src);
    i = 0;
    while (i < size) {
        loc = strchr(b64table, src[i]);
        if (loc == (char *)0)
            break;
        else
            a[i] = loc - b64table;
        ++i;
    }
    /* if nothing valid to process we have a zero length response */
    if (i == 0)
        return 0;
    size = i;
    i = size - 1;
    j = size;
    while (1) {
        a[j] = a[i];
        if (--i < 0)
            break;
        a[j] |= (a[i] & 3) << 6;
        --j;
        a[j] = (unsigned char)((a[i] & 0x3c) >> 2);
        if (--i < 0)
            break;
        a[j] |= (a[i] & 0xf) << 4;
        --j;
        a[j] = (unsigned char)((a[i] & 0x30) >> 4);
        if (--i < 0)
            break;
        a[j] |= (a[i] << 2);

        a[--j] = 0;
        if (--i < 0)
            break;
    }
    while (a[j] == 0 && j <= size)
        ++j;
    i = 0;
    while (j <= size)
        a[i++] = a[j++];
    return i;
}