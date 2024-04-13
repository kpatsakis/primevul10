static void test_read_id(void)
{
    uint8_t drive = 0;
    uint8_t head = 0;
    uint8_t cyl;
    uint8_t st0;
    uint8_t msr;

    /* Seek to track 0 and check with READ ID */
    send_seek(0);

    floppy_send(CMD_READ_ID);
    g_assert(!get_irq(FLOPPY_IRQ));
    floppy_send(head << 2 | drive);

    msr = inb(FLOPPY_BASE + reg_msr);
    if (!get_irq(FLOPPY_IRQ)) {
        assert_bit_set(msr, BUSY);
        assert_bit_clear(msr, RQM);
    }

    while (!get_irq(FLOPPY_IRQ)) {
        /* qemu involves a timer with READ ID... */
        clock_step(1000000000LL / 50);
    }

    msr = inb(FLOPPY_BASE + reg_msr);
    assert_bit_set(msr, BUSY | RQM | DIO);

    st0 = floppy_recv();
    floppy_recv();
    floppy_recv();
    cyl = floppy_recv();
    head = floppy_recv();
    floppy_recv();
    g_assert(get_irq(FLOPPY_IRQ));
    floppy_recv();
    g_assert(!get_irq(FLOPPY_IRQ));

    g_assert_cmpint(cyl, ==, 0);
    g_assert_cmpint(head, ==, 0);
    g_assert_cmpint(st0, ==, head << 2);

    /* Seek to track 8 on head 1 and check with READ ID */
    head = 1;
    cyl = 8;

    floppy_send(CMD_SEEK);
    floppy_send(head << 2 | drive);
    g_assert(!get_irq(FLOPPY_IRQ));
    floppy_send(cyl);
    g_assert(get_irq(FLOPPY_IRQ));
    ack_irq(NULL);

    floppy_send(CMD_READ_ID);
    g_assert(!get_irq(FLOPPY_IRQ));
    floppy_send(head << 2 | drive);

    msr = inb(FLOPPY_BASE + reg_msr);
    if (!get_irq(FLOPPY_IRQ)) {
        assert_bit_set(msr, BUSY);
        assert_bit_clear(msr, RQM);
    }

    while (!get_irq(FLOPPY_IRQ)) {
        /* qemu involves a timer with READ ID... */
        clock_step(1000000000LL / 50);
    }

    msr = inb(FLOPPY_BASE + reg_msr);
    assert_bit_set(msr, BUSY | RQM | DIO);

    st0 = floppy_recv();
    floppy_recv();
    floppy_recv();
    cyl = floppy_recv();
    head = floppy_recv();
    floppy_recv();
    g_assert(get_irq(FLOPPY_IRQ));
    floppy_recv();
    g_assert(!get_irq(FLOPPY_IRQ));

    g_assert_cmpint(cyl, ==, 8);
    g_assert_cmpint(head, ==, 1);
    g_assert_cmpint(st0, ==, head << 2);
}