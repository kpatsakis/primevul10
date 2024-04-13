static uint16_t u16in(void)
{
    uint16_t u16;
    datain(&u16, 2);
    u16 = bswap16(u16);
    return u16;
}