static bool unassigned_mem_accepts(struct uc_struct *uc, void *opaque, hwaddr addr,
                                   unsigned size, bool is_write,
                                   MemTxAttrs attrs)
{
    return false;
}