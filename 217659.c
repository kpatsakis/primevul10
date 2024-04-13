put_u32(struct ofpbuf *b, uint32_t x)
{
    put_be32(b, htonl(x));
}