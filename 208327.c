void acpi_os_set_prepare_sleep(int (*func)(u8 sleep_state,
			       u32 pm1a_ctrl, u32 pm1b_ctrl))
{
	__acpi_os_prepare_sleep = func;
}