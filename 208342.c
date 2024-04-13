static void __iomem *acpi_map(acpi_physical_address pg_off, unsigned long pg_sz)
{
	unsigned long pfn;

	pfn = pg_off >> PAGE_SHIFT;
	if (should_use_kmap(pfn)) {
		if (pg_sz > PAGE_SIZE)
			return NULL;
		return (void __iomem __force *)kmap(pfn_to_page(pfn));
	} else
		return acpi_os_ioremap(pg_off, pg_sz);
}