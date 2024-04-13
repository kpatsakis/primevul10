acpi_physical_address __init acpi_os_get_root_pointer(void)
{
#ifdef CONFIG_KEXEC
	if (acpi_rsdp && (get_securelevel() <= 0))
		return acpi_rsdp;
#endif

	if (efi_enabled(EFI_CONFIG_TABLES)) {
		if (efi.acpi20 != EFI_INVALID_TABLE_ADDR)
			return efi.acpi20;
		else if (efi.acpi != EFI_INVALID_TABLE_ADDR)
			return efi.acpi;
		else {
			printk(KERN_ERR PREFIX
			       "System description tables not found\n");
			return 0;
		}
	} else if (IS_ENABLED(CONFIG_ACPI_LEGACY_TABLES_LOOKUP)) {
		acpi_physical_address pa = 0;

		acpi_find_root_pointer(&pa);
		return pa;
	}

	return 0;
}