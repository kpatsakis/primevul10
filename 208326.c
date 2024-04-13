acpi_status acpi_os_prepare_sleep(u8 sleep_state, u32 pm1a_control,
				  u32 pm1b_control)
{
	int rc = 0;
	if (__acpi_os_prepare_sleep)
		rc = __acpi_os_prepare_sleep(sleep_state,
					     pm1a_control, pm1b_control);
	if (rc < 0)
		return AE_ERROR;
	else if (rc > 0)
		return AE_CTRL_SKIP;

	return AE_OK;
}