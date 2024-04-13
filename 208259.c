ssize_t acpi_debugger_read_cmd(char *buffer, size_t buffer_length)
{
	ssize_t ret;
	ssize_t (*func)(char *, size_t);
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
	func = acpi_debugger.ops->read_cmd;
	owner = acpi_debugger.owner;
	mutex_unlock(&acpi_debugger.lock);

	ret = func(buffer, buffer_length);

	mutex_lock(&acpi_debugger.lock);
	module_put(owner);
err_lock:
	mutex_unlock(&acpi_debugger.lock);
	return ret;
}