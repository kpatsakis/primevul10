void __ref acpi_os_unmap_iomem(void __iomem *virt, acpi_size size)
{
	struct acpi_ioremap *map;

	if (!acpi_gbl_permanent_mmap) {
		__acpi_unmap_table(virt, size);
		return;
	}

	mutex_lock(&acpi_ioremap_lock);
	map = acpi_map_lookup_virt(virt, size);
	if (!map) {
		mutex_unlock(&acpi_ioremap_lock);
		WARN(true, PREFIX "%s: bad address %p\n", __func__, virt);
		return;
	}
	acpi_os_drop_map_ref(map);
	mutex_unlock(&acpi_ioremap_lock);

	acpi_os_map_cleanup(map);
}