static void test_read_no_dma_18(void)
{
    uint8_t ret;

    outb(FLOPPY_BASE + reg_dor, inb(FLOPPY_BASE + reg_dor) & ~0x08);
    send_seek(0);
    ret = send_read_no_dma_command(18, 0x04);
    g_assert(ret == 0);
}