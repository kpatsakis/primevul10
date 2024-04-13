int acpi_debugger_create_thread(acpi_osd_exec_callback function, void *context)
{
	int ret;
	int (*func)(acpi_osd_exec_callback, void *);
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
	func = acpi_debugger.ops->create_thread;
	owner = acpi_debugger.owner;
	mutex_unlock(&acpi_debugger.lock);

	ret = func(function, context);

	mutex_lock(&acpi_debugger.lock);
	module_put(owner);
err_lock:
	mutex_unlock(&acpi_debugger.lock);
	return ret;
}