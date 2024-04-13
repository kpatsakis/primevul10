int __init acpi_debugger_init(void)
{
	mutex_init(&acpi_debugger.lock);
	acpi_debugger_initialized = true;
	return 0;
}