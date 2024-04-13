bool cpu_physical_memory_rw(AddressSpace *as, hwaddr addr, void *buf,
                            hwaddr len, bool is_write)
{
    MemTxResult result = address_space_rw(as, addr, MEMTXATTRS_UNSPECIFIED,
                     buf, len, is_write);
    if (result == MEMTX_OK) {
        return true;
    } else {
        return false;
    }
}