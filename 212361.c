static uint32_t u32in(void)
{
    uint32_t u32;
    datain(&u32, 4);
    u32 = bswap32(u32);
    return u32;
}