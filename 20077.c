int cpu_memory_rw_debug(CPUState *cpu, target_ulong addr,
                        void *ptr, target_ulong len, bool is_write)
{
#ifdef TARGET_ARM
    struct uc_struct *uc = cpu->uc;
#endif
    hwaddr phys_addr;
    target_ulong l, page;
    uint8_t *buf = ptr;

    while (len > 0) {
        int asidx;
        MemTxAttrs attrs;

        page = addr & TARGET_PAGE_MASK;
        phys_addr = cpu_get_phys_page_attrs_debug(cpu, page, &attrs);
        asidx = cpu_asidx_from_attrs(cpu, attrs);
        /* if no physical page mapped, return an error */
        if (phys_addr == -1)
            return -1;
        l = (page + TARGET_PAGE_SIZE) - addr;
        if (l > len)
            l = len;
        phys_addr += (addr & ~TARGET_PAGE_MASK);
        if (is_write) {
            address_space_write_rom(cpu->cpu_ases[asidx].as, phys_addr,
                                    attrs, buf, l);
        } else {
            address_space_read(cpu->cpu_ases[asidx].as, phys_addr, attrs, buf,
                               l);
        }
        len -= l;
        buf += l;
        addr += l;
    }
    return 0;
}