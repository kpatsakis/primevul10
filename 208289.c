void acpi_os_unmap_generic_address(struct acpi_generic_address *gas)
{
	u64 addr;
	struct acpi_ioremap *map;

	if (gas->space_id != ACPI_ADR_SPACE_SYSTEM_MEMORY)
		return;

	/* Handle possible alignment issues */
	memcpy(&addr, &gas->address, sizeof(addr));
	if (!addr || !gas->bit_width)
		return;

	mutex_lock(&acpi_ioremap_lock);
	map = acpi_map_lookup(addr, gas->bit_width / 8);
	if (!map) {
		mutex_unlock(&acpi_ioremap_lock);
		return;
	}
	acpi_os_drop_map_ref(map);
	mutex_unlock(&acpi_ioremap_lock);

	acpi_os_map_cleanup(map);
}