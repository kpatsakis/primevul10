static inline uint8_t *get_root_table(uint32_t dmar_index)
{
	static struct page root_tables[CONFIG_MAX_IOMMU_NUM] __aligned(PAGE_SIZE);
	return root_tables[dmar_index].contents;
}