static u32 acpi_osi_handler(acpi_string interface, u32 supported)
{
	if (!strcmp("Linux", interface)) {

		printk_once(KERN_NOTICE FW_BUG PREFIX
			"BIOS _OSI(Linux) query %s%s\n",
			osi_linux.enable ? "honored" : "ignored",
			osi_linux.cmdline ? " via cmdline" :
			osi_linux.dmi ? " via DMI" : "");
	}

	if (!strcmp("Darwin", interface)) {
		/*
		 * Apple firmware will behave poorly if it receives positive
		 * answers to "Darwin" and any other OS. Respond positively
		 * to Darwin and then disable all other vendor strings.
		 */
		acpi_update_interfaces(ACPI_DISABLE_ALL_VENDOR_STRINGS);
		supported = ACPI_UINT32_MAX;
	}

	return supported;
}