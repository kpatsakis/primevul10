acpi_os_table_override(struct acpi_table_header *existing_table,
		       struct acpi_table_header **new_table)
{
	if (!existing_table || !new_table)
		return AE_BAD_PARAMETER;

	*new_table = NULL;

#ifdef CONFIG_ACPI_CUSTOM_DSDT
	if (strncmp(existing_table->signature, "DSDT", 4) == 0)
		*new_table = (struct acpi_table_header *)AmlCode;
#endif
	if (*new_table != NULL)
		acpi_table_taint(existing_table);
	return AE_OK;
}