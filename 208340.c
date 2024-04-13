static void __init acpi_request_region (struct acpi_generic_address *gas,
	unsigned int length, char *desc)
{
	u64 addr;

	/* Handle possible alignment issues */
	memcpy(&addr, &gas->address, sizeof(addr));
	if (!addr || !length)
		return;

	/* Resources are never freed */
	if (gas->space_id == ACPI_ADR_SPACE_SYSTEM_IO)
		request_region(addr, length, desc);
	else if (gas->space_id == ACPI_ADR_SPACE_SYSTEM_MEMORY)
		request_mem_region(addr, length, desc);
}