static MemoryRegionSection flatview_do_translate(struct uc_struct *uc, FlatView *fv,
                                                 hwaddr addr,
                                                 hwaddr *xlat,
                                                 hwaddr *plen_out,
                                                 hwaddr *page_mask_out,
                                                 bool is_write,
                                                 bool is_mmio,
                                                 AddressSpace **target_as,
                                                 MemTxAttrs attrs)
{
    MemoryRegionSection *section;
    IOMMUMemoryRegion *iommu_mr;
    hwaddr plen = (hwaddr)(-1);

    if (!plen_out) {
        plen_out = &plen;
    }

    section = address_space_translate_internal(
            flatview_to_dispatch(fv), addr, xlat,
            plen_out, is_mmio);

    iommu_mr = memory_region_get_iommu(section->mr);
    if (unlikely(iommu_mr)) {
        return address_space_translate_iommu(iommu_mr, xlat,
                                             plen_out, page_mask_out,
                                             is_write, is_mmio,
                                             target_as, attrs);
    }
    if (page_mask_out) {
        /* Not behind an IOMMU, use default page size. */
        *page_mask_out = ~TARGET_PAGE_MASK;
    }

    return *section;
}