void __init early_acpi_os_unmap_memory(void __iomem *virt, acpi_size size)
{
	if (!acpi_gbl_permanent_mmap)
		__acpi_unmap_table(virt, size);
}