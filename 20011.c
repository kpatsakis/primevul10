void *cpu_physical_memory_map(AddressSpace *as, hwaddr addr,
                              hwaddr *plen,
                              bool is_write)
{
    return address_space_map(as, addr, plen, is_write,
                             MEMTXATTRS_UNSPECIFIED);
}