put_u16(struct ofpbuf *b, uint16_t x)
{
    put_be16(b, htons(x));
}