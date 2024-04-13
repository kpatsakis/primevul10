void __iomem *acpi_os_get_iomem(acpi_physical_address phys, unsigned int size)
{
	struct acpi_ioremap *map;
	void __iomem *virt = NULL;

	mutex_lock(&acpi_ioremap_lock);
	map = acpi_map_lookup(phys, size);
	if (map) {
		virt = map->virt + (phys - map->phys);
		map->refcount++;
	}
	mutex_unlock(&acpi_ioremap_lock);
	return virt;
}