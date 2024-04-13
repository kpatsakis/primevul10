static int ebcdic_new(BIO *bi)
{
    EBCDIC_OUTBUFF *wbuf;

    wbuf = app_malloc(sizeof(*wbuf) + 1024, "ebcdic wbuf");
    wbuf->alloced = 1024;
    wbuf->buff[0] = '\0';

    bi->ptr = (char *)wbuf;
    bi->init = 1;
    bi->flags = 0;
    return (1);
}