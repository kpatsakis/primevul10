static void test_mem_protect_map_ptr(void)
{
    uc_engine *uc;
    uint64_t val = 0x114514;
    uint8_t *data1 = NULL;
    uint8_t *data2 = NULL;
    uint64_t mem;

    data1 = calloc(sizeof(*data1), 0x4000);
    data2 = calloc(sizeof(*data2), 0x2000);

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

    OK(uc_mem_map_ptr(uc, 0x4000, 0x4000, UC_PROT_ALL, data1));
    OK(uc_mem_unmap(uc, 0x6000, 0x2000));
    OK(uc_mem_map_ptr(uc, 0x6000, 0x2000, UC_PROT_ALL, data2));

    OK(uc_mem_write(uc, 0x6004, &val, 8));
    OK(uc_mem_protect(uc, 0x6000, 0x1000, UC_PROT_READ));
    OK(uc_mem_read(uc, 0x6004, (void *)&mem, 8));

    TEST_CHECK(val == mem);

    OK(uc_close(uc));
}