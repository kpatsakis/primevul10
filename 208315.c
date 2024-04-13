void acpi_os_vprintf(const char *fmt, va_list args)
{
	static char buffer[512];

	vsprintf(buffer, fmt, args);

#ifdef ENABLE_DEBUGGER
	if (acpi_in_debugger) {
		kdb_printf("%s", buffer);
	} else {
		printk(KERN_CONT "%s", buffer);
	}
#else
	if (acpi_debugger_write_log(buffer) < 0)
		printk(KERN_CONT "%s", buffer);
#endif
}