static MemTxResult subpage_write(struct uc_struct *uc, void *opaque, hwaddr addr,
                                 uint64_t value, unsigned len, MemTxAttrs attrs)
{
    subpage_t *subpage = opaque;
    uint8_t buf[8];

#if defined(DEBUG_SUBPAGE)
    printf("%s: subpage %p len %u addr " TARGET_FMT_plx
           " value %"PRIx64"\n",
           __func__, subpage, len, addr, value);
#endif
    stn_p(buf, len, value);
    return flatview_write(uc, subpage->fv, addr + subpage->base, attrs, buf, len);
}