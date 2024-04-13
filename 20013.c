address_space_translate_for_iotlb(CPUState *cpu, int asidx, hwaddr addr,
                                  hwaddr *xlat, hwaddr *plen,
                                  MemTxAttrs attrs, int *prot)
{
    MemoryRegionSection *section;
    IOMMUMemoryRegion *iommu_mr;
    IOMMUMemoryRegionClass *imrc;
    IOMMUTLBEntry iotlb;
    int iommu_idx;
    AddressSpaceDispatch *d = cpu->cpu_ases[asidx].memory_dispatch;

    for (;;) {
        section = address_space_translate_internal(d, addr, &addr, plen, false);

        iommu_mr = memory_region_get_iommu(section->mr);
        if (!iommu_mr) {
            break;
        }

        imrc = memory_region_get_iommu_class_nocheck(iommu_mr);

        iommu_idx = imrc->attrs_to_index(iommu_mr, attrs);

        // tcg_register_iommu_notifier(cpu, iommu_mr, iommu_idx);

        /* We need all the permissions, so pass IOMMU_NONE so the IOMMU
         * doesn't short-cut its translation table walk.
         */
        iotlb = imrc->translate(iommu_mr, addr, IOMMU_NONE, iommu_idx);
        addr = ((iotlb.translated_addr & ~iotlb.addr_mask)
                | (addr & iotlb.addr_mask));
        /* Update the caller's prot bits to remove permissions the IOMMU
         * is giving us a failure response for. If we get down to no
         * permissions left at all we can give up now.
         */
        if (!(iotlb.perm & IOMMU_RO)) {
            *prot &= ~(PAGE_READ | PAGE_EXEC);
        }
        if (!(iotlb.perm & IOMMU_WO)) {
            *prot &= ~PAGE_WRITE;
        }

        if (!*prot) {
            goto translate_fail;
        }

        d = flatview_to_dispatch(address_space_to_flatview(iotlb.target_as));
    }

    assert(!(memory_region_get_iommu(section->mr) != NULL));
    *xlat = addr;
    // Unicorn:
    //   If there is no memory mapped but still we start emulation, we will get
    //   a default memory region section and it would be marked as an IO memory
    //   in cputlb which prevents further fecthing and execution.
    //
    //   The reason we set prot to 0 here is not to setting protection but to notify
    //   the outer function to add a new **blank** tlb which will never be hitted.
    if (!memory_region_is_ram(section->mr) && section == &d->map.sections[PHYS_SECTION_UNASSIGNED]) {
        *prot = 0;
    }
    return section;

translate_fail:
    return &d->map.sections[PHYS_SECTION_UNASSIGNED];
}