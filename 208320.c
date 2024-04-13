void acpi_os_set_prepare_extended_sleep(int (*func)(u8 sleep_state,
			       u32 val_a, u32 val_b))
{
	__acpi_os_prepare_extended_sleep = func;
}