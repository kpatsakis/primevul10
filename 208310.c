static void __init acpi_cmdline_osi_linux(unsigned int enable)
{
	osi_linux.cmdline = 1;	/* cmdline set the default and override DMI */
	osi_linux.dmi = 0;
	set_osi_linux(enable);

	return;
}