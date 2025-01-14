fmtstr(char **sbuffer,
       char **buffer,
       size_t *currlen,
       size_t *maxlen, const char *value, int flags, int min, int max)
{
    int padlen;
    size_t strln;
    int cnt = 0;

    if (value == 0)
        value = "<NULL>";

    strln = strlen(value);
    if (strln > INT_MAX)
        strln = INT_MAX;

    padlen = min - strln;
    if (min < 0 || padlen < 0)
        padlen = 0;
    if (flags & DP_F_MINUS)
        padlen = -padlen;

    while ((padlen > 0) && (cnt < max)) {
        if(!doapr_outch(sbuffer, buffer, currlen, maxlen, ' '))
            return 0;
        --padlen;
        ++cnt;
    }
    while (*value && (cnt < max)) {
        if(!doapr_outch(sbuffer, buffer, currlen, maxlen, *value++))
            return 0;
        ++cnt;
    }
    while ((padlen < 0) && (cnt < max)) {
        if(!doapr_outch(sbuffer, buffer, currlen, maxlen, ' '))
            return 0;
        ++padlen;
        ++cnt;
    }
    return 1;
}