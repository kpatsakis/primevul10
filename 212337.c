static uint32_t getsize(void)
{
    int cnt;
    uint32_t size = 0;
    for (cnt = 0; cnt < 4; cnt++)
    {
        int tmp = u8in();

        size <<= 7;
        size |= (tmp & 0x7f);
        if (!(tmp & 0x80))
            break;
    }
    return size;
}