acpi_status acpi_os_get_line(char *buffer, u32 buffer_length, u32 *bytes_read)
{
#ifdef ENABLE_DEBUGGER
	if (acpi_in_debugger) {
		u32 chars;

		kdb_read(buffer, buffer_length);

		/* remove the CR kdb includes */
		chars = strlen(buffer) - 1;
		buffer[chars] = '\0';
	}
#else
	int ret;

	ret = acpi_debugger_read_cmd(buffer, buffer_length);
	if (ret < 0)
		return AE_ERROR;
	if (bytes_read)
		*bytes_read = ret;
#endif

	return AE_OK;
}