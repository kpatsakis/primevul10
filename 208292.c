static int __init acpi_no_auto_serialize_setup(char *str)
{
	acpi_gbl_auto_serialize_methods = FALSE;
	pr_info("ACPI: auto-serialization disabled\n");

	return 1;
}