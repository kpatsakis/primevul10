static void mmio_write_wrapper(struct uc_struct *uc, void *opaque, hwaddr addr, uint64_t data, unsigned size)
{
    mmio_cbs* cbs = (mmio_cbs*)opaque;
    
    // We have to care about 32bit target.
    addr = addr & ( (target_ulong)(-1) );
    if (cbs->write) {
        cbs->write(uc, addr, size, data, cbs->user_data_write);
    }
}