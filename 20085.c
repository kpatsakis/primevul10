static void test_map_correct(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_mem_map(uc, 0x40000, 0x1000 * 16, UC_PROT_ALL)); // [0x40000, 0x50000]
    OK(uc_mem_map(uc, 0x60000, 0x1000 * 16, UC_PROT_ALL)); // [0x60000, 0x70000]
    OK(uc_mem_map(uc, 0x20000, 0x1000 * 16, UC_PROT_ALL)); // [0x20000, 0x30000]
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x10000, 0x2000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x25000, 0x1000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x35000, 0x1000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x45000, 0x1000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x55000, 0x2000 * 16, UC_PROT_ALL));
    OK(uc_mem_map(uc, 0x35000, 0x5000, UC_PROT_ALL));
    OK(uc_mem_map(uc, 0x50000, 0x5000, UC_PROT_ALL));

    OK(uc_close(uc));
}