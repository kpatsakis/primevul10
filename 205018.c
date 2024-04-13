static int ebcdic_puts(BIO *bp, const char *str)
{
    if (bp->next_bio == NULL)
        return (0);
    return ebcdic_write(bp, str, strlen(str));
}