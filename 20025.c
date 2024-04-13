static uint64_t mmio_read_wrapper(struct uc_struct *uc, void *opaque, hwaddr addr, unsigned size)
{
    mmio_cbs* cbs = (mmio_cbs*)opaque;
    
    // We have to care about 32bit target.
    addr = addr & ( (target_ulong)(-1) );
    if (cbs->read) {
        return cbs->read(uc, addr, size, cbs->user_data_read);
    } else {
        return 0;
    }
}