static void fault_record_analysis(__unused uint64_t low, uint64_t high)
{
	union pci_bdf dmar_bdf;

	if (!dma_frcd_up_f(high)) {
		dmar_bdf.value = dma_frcd_up_sid(high);
		/* currently skip PASID related parsing */
		pr_info("%s, Reason: 0x%x, SID: %x.%x.%x @0x%lx",
			(dma_frcd_up_t(high) != 0U) ? "Read/Atomic" : "Write", dma_frcd_up_fr(high),
			dmar_bdf.bits.b, dmar_bdf.bits.d, dmar_bdf.bits.f, low);
#if DBG_IOMMU
		if (iommu_ecap_dt(dmar_unit->ecap) != 0U) {
			pr_info("Address Type: 0x%x", dma_frcd_up_at(high));
		}
#endif
	}
}