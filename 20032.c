static subpage_t *subpage_init(struct uc_struct *uc, FlatView *fv, hwaddr base)
{
    subpage_t *mmio;

    /* mmio->sub_section is set to PHYS_SECTION_UNASSIGNED with g_malloc0 */
    mmio = g_malloc0(sizeof(subpage_t) + TARGET_PAGE_SIZE * sizeof(uint16_t));
    mmio->fv = fv;
    mmio->base = base;
    memory_region_init_io(fv->root->uc, &mmio->iomem, &subpage_ops, mmio,
                          TARGET_PAGE_SIZE);
    mmio->iomem.subpage = true;
#if defined(DEBUG_SUBPAGE)
    printf("%s: %p base " TARGET_FMT_plx " len %08x\n", __func__,
           mmio, base, TARGET_PAGE_SIZE);
#endif

    return mmio;
}