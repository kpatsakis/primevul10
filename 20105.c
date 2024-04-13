static void test_splitting_mem_unmap(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_32, &uc));

    OK(uc_mem_map(uc, 0x20000, 0x1000, UC_PROT_NONE));
    OK(uc_mem_map(uc, 0x21000, 0x2000, UC_PROT_NONE));

    OK(uc_mem_unmap(uc, 0x21000, 0x1000));

    OK(uc_close(uc));
}