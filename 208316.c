acpi_status acpi_os_notify_command_complete(void)
{
	int ret;

	ret = acpi_debugger_notify_command_complete();
	if (ret < 0)
		return AE_ERROR;
	return AE_OK;
}