int acpi_debugger_wait_command_ready(void)
{
	int ret;
	int (*func)(bool, char *, size_t);
	struct module *owner;

	if (!acpi_debugger_initialized)
		return -ENODEV;
	mutex_lock(&acpi_debugger.lock);
	if (!acpi_debugger.ops) {
		ret = -ENODEV;
		goto err_lock;
	}
	if (!try_module_get(acpi_debugger.owner)) {
		ret = -ENODEV;
		goto err_lock;
	}
	func = acpi_debugger.ops->wait_command_ready;
	owner = acpi_debugger.owner;
	mutex_unlock(&acpi_debugger.lock);

	ret = func(acpi_gbl_method_executing,
		   acpi_gbl_db_line_buf, ACPI_DB_LINE_BUFFER_SIZE);

	mutex_lock(&acpi_debugger.lock);
	module_put(owner);
err_lock:
	mutex_unlock(&acpi_debugger.lock);
	return ret;
}