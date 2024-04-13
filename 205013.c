static int ebcdic_write(BIO *b, const char *in, int inl)
{
    EBCDIC_OUTBUFF *wbuf;
    int ret = 0;
    int num;
    unsigned char n;

    if ((in == NULL) || (inl <= 0))
        return (0);
    if (b->next_bio == NULL)
        return (0);

    wbuf = (EBCDIC_OUTBUFF *) b->ptr;

    if (inl > (num = wbuf->alloced)) {
        num = num + num;        /* double the size */
        if (num < inl)
            num = inl;
        wbuf = app_malloc(sizeof(*wbuf) + num, "grow ebcdic wbuf");
        OPENSSL_free(b->ptr);

        wbuf->alloced = num;
        wbuf->buff[0] = '\0';

        b->ptr = (char *)wbuf;
    }

    ebcdic2ascii(wbuf->buff, in, inl);

    ret = BIO_write(b->next_bio, wbuf->buff, inl);

    return (ret);
}