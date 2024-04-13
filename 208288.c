static int __init acpi_no_static_ssdt_setup(char *s)
{
	acpi_gbl_disable_ssdt_table_install = TRUE;
	pr_info("ACPI: static SSDT installation disabled\n");

	return 0;
}