bool cpu_physical_memory_is_io(AddressSpace *as, hwaddr phys_addr)
{
    MemoryRegion*mr;
    hwaddr l = 1;
    bool res;

    mr = address_space_translate(as,
                                 phys_addr, &phys_addr, &l, false,
                                 MEMTXATTRS_UNSPECIFIED);

    res = !memory_region_is_ram(mr);
    return res;
}