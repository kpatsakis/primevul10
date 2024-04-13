void __ref acpi_os_unmap_memory(void *virt, acpi_size size)
{
	return acpi_os_unmap_iomem((void __iomem *)virt, size);
}