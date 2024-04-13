static int hdlr2in(int size)
{
    uint8_t buf[4];

    // version/flags
    u32in();
    // Predefined
    u32in();
    // Handler type
    datain(buf, 4);
    if (memcmp(buf, "mdir", 4))
        return ERR_FAIL;
    datain(buf, 4);
    if (memcmp(buf, "appl", 4))
        return ERR_FAIL;
    // Reserved
    u32in();
    u32in();
    // null terminator
    u8in();

    return size;
};