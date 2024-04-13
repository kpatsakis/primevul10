static void __init acpi_osi_setup_late(void)
{
	struct osi_setup_entry *osi;
	char *str;
	int i;
	acpi_status status;

	if (osi_linux.default_disabling) {
		status = acpi_update_interfaces(ACPI_DISABLE_ALL_VENDOR_STRINGS);

		if (ACPI_SUCCESS(status))
			printk(KERN_INFO PREFIX "Disabled all _OSI OS vendors\n");
	}

	for (i = 0; i < OSI_STRING_ENTRIES_MAX; i++) {
		osi = &osi_setup_entries[i];
		str = osi->string;

		if (*str == '\0')
			break;
		if (osi->enable) {
			status = acpi_install_interface(str);

			if (ACPI_SUCCESS(status))
				printk(KERN_INFO PREFIX "Added _OSI(%s)\n", str);
		} else {
			status = acpi_remove_interface(str);

			if (ACPI_SUCCESS(status))
				printk(KERN_INFO PREFIX "Deleted _OSI(%s)\n", str);
		}
	}
}