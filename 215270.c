static void suspend_dmar(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t i;

	dmar_invalid_context_cache_global(dmar_unit);
	dmar_invalid_iotlb_global(dmar_unit);
	dmar_invalid_iec_global(dmar_unit);

	disable_dmar(dmar_unit);

	/* save IOMMU fault register state */
	for (i = 0U; i < IOMMU_FAULT_REGISTER_STATE_NUM; i++) {
		dmar_unit->fault_state[i] =  iommu_read32(dmar_unit, DMAR_FECTL_REG + (i * IOMMU_FAULT_REGISTER_SIZE));
	}
}