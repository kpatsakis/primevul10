void acpi_os_delete_lock(acpi_spinlock handle)
{
	ACPI_FREE(handle);
}