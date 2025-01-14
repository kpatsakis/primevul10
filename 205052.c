doapr_outch(char **sbuffer,
            char **buffer, size_t *currlen, size_t *maxlen, int c)
{
    /* If we haven't at least one buffer, someone has doe a big booboo */
    assert(*sbuffer != NULL || buffer != NULL);

    /* |currlen| must always be <= |*maxlen| */
    assert(*currlen <= *maxlen);

    if (buffer && *currlen == *maxlen) {
        if (*maxlen > INT_MAX - BUFFER_INC)
            return 0;

        *maxlen += BUFFER_INC;
        if (*buffer == NULL) {
            *buffer = OPENSSL_malloc(*maxlen);
            if (*buffer == NULL)
                return 0;
            if (*currlen > 0) {
                assert(*sbuffer != NULL);
                memcpy(*buffer, *sbuffer, *currlen);
            }
            *sbuffer = NULL;
        } else {
            char *tmpbuf;
            tmpbuf = OPENSSL_realloc(*buffer, *maxlen);
            if (tmpbuf == NULL)
                return 0;
            *buffer = tmpbuf;
        }
    }

    if (*currlen < *maxlen) {
        if (*sbuffer)
            (*sbuffer)[(*currlen)++] = (char)c;
        else
            (*buffer)[(*currlen)++] = (char)c;
    }

    return 1;
}