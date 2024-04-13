acpi_status __init acpi_os_initialize(void)
{
	acpi_os_map_generic_address(&acpi_gbl_FADT.xpm1a_event_block);
	acpi_os_map_generic_address(&acpi_gbl_FADT.xpm1b_event_block);
	acpi_os_map_generic_address(&acpi_gbl_FADT.xgpe0_block);
	acpi_os_map_generic_address(&acpi_gbl_FADT.xgpe1_block);
	if (acpi_gbl_FADT.flags & ACPI_FADT_RESET_REGISTER) {
		/*
		 * Use acpi_os_map_generic_address to pre-map the reset
		 * register if it's in system memory.
		 */
		int rv;

		rv = acpi_os_map_generic_address(&acpi_gbl_FADT.reset_register);
		pr_debug(PREFIX "%s: map reset_reg status %d\n", __func__, rv);
	}
	acpi_os_initialized = true;

	return AE_OK;
}