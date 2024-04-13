static int __init setup_acpi_rsdp(char *arg)
{
	if (kstrtoul(arg, 16, &acpi_rsdp))
		return -EINVAL;
	return 0;
}