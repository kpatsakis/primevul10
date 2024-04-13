static bool dmar_unit_support_aw(const struct dmar_drhd_rt *dmar_unit, uint32_t addr_width)
{
	uint8_t aw;

	aw = width_to_agaw(addr_width);

	return (((1U << aw) & iommu_cap_sagaw(dmar_unit->cap)) != 0U);
}