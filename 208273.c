static void __init set_osi_linux(unsigned int enable)
{
	if (osi_linux.enable != enable)
		osi_linux.enable = enable;

	if (osi_linux.enable)
		acpi_osi_setup("Linux");
	else
		acpi_osi_setup("!Linux");

	return;
}