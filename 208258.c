void acpi_unregister_debugger(const struct acpi_debugger_ops *ops)
{
	mutex_lock(&acpi_debugger.lock);
	if (ops == acpi_debugger.ops) {
		acpi_debugger.ops = NULL;
		acpi_debugger.owner = NULL;
	}
	mutex_unlock(&acpi_debugger.lock);
}