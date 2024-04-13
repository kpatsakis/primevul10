void tb_invalidate_phys_addr(AddressSpace *as, hwaddr addr, MemTxAttrs attrs)
{
    ram_addr_t ram_addr;
    MemoryRegion *mr;
    hwaddr l = 1;

    mr = address_space_translate(as, addr, &addr, &l, false, attrs);
    if (!memory_region_is_ram(mr)) {
        return;
    }

    ram_addr = memory_region_get_ram_addr(mr) + addr;
    tb_invalidate_phys_page_range(as->uc, ram_addr, ram_addr + 1);
}