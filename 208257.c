acpi_status acpi_os_wait_command_ready(void)
{
	int ret;

	ret = acpi_debugger_wait_command_ready();
	if (ret < 0)
		return AE_ERROR;
	return AE_OK;
}