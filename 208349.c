static int __init osi_setup(char *str)
{
	if (str && !strcmp("Linux", str))
		acpi_cmdline_osi_linux(1);
	else if (str && !strcmp("!Linux", str))
		acpi_cmdline_osi_linux(0);
	else
		acpi_osi_setup(str);

	return 1;
}