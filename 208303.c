int __init acpi_rev_override_setup(char *str)
{
	acpi_rev_override = true;
	return 1;
}