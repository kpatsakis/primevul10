acpi_status acpi_os_prepare_extended_sleep(u8 sleep_state, u32 val_a,
				  u32 val_b)
{
	int rc = 0;
	if (__acpi_os_prepare_extended_sleep)
		rc = __acpi_os_prepare_extended_sleep(sleep_state,
					     val_a, val_b);
	if (rc < 0)
		return AE_ERROR;
	else if (rc > 0)
		return AE_CTRL_SKIP;

	return AE_OK;
}