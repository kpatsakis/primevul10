void __init acpi_dmi_osi_linux(int enable, const struct dmi_system_id *d)
{
	printk(KERN_NOTICE PREFIX "DMI detected: %s\n", d->ident);

	if (enable == -1)
		return;

	osi_linux.dmi = 1;	/* DMI knows that this box asks OSI(Linux) */
	set_osi_linux(enable);

	return;
}