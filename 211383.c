static int __init acpi_parse_hpet(unsigned long phys, unsigned long size)
{
	struct acpi_table_hpet *hpet_tbl;

	if (!phys || !size)
		return -EINVAL;

	hpet_tbl = (struct acpi_table_hpet *)__acpi_map_table(phys, size);
	if (!hpet_tbl) {
		printk(KERN_WARNING PREFIX "Unable to map HPET\n");
		return -ENODEV;
	}

	if (hpet_tbl->addr.space_id != ACPI_SPACE_MEM) {
		printk(KERN_WARNING PREFIX "HPET timers must be located in "
		       "memory.\n");
		return -1;
	}
#ifdef	CONFIG_X86_64
	vxtime.hpet_address = hpet_tbl->addr.addrl |
	    ((long)hpet_tbl->addr.addrh << 32);

	printk(KERN_INFO PREFIX "HPET id: %#x base: %#lx\n",
	       hpet_tbl->id, vxtime.hpet_address);
#else				/* X86 */
	{
		extern unsigned long hpet_address;

		hpet_address = hpet_tbl->addr.addrl;
		printk(KERN_INFO PREFIX "HPET id: %#x base: %#lx\n",
		       hpet_tbl->id, hpet_address);
	}
#endif				/* X86 */

	return 0;
}