static int __init acpi_disable_return_repair(char *s)
{
	printk(KERN_NOTICE PREFIX
	       "ACPI: Predefined validation mechanism disabled\n");
	acpi_gbl_disable_auto_repair = TRUE;

	return 1;
}