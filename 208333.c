acpi_map_lookup(acpi_physical_address phys, acpi_size size)
{
	struct acpi_ioremap *map;

	list_for_each_entry_rcu(map, &acpi_ioremaps, list)
		if (map->phys <= phys &&
		    phys + size <= map->phys + map->size)
			return map;

	return NULL;
}