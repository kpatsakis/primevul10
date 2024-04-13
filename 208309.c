acpi_map_vaddr_lookup(acpi_physical_address phys, unsigned int size)
{
	struct acpi_ioremap *map;

	map = acpi_map_lookup(phys, size);
	if (map)
		return map->virt + (phys - map->phys);

	return NULL;
}