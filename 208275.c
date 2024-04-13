acpi_os_map_memory(acpi_physical_address phys, acpi_size size)
{
	return (void *)acpi_os_map_iomem(phys, size);
}