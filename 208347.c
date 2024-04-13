int acpi_register_debugger(struct module *owner,
			   const struct acpi_debugger_ops *ops)
{
	int ret = 0;

	mutex_lock(&acpi_debugger.lock);
	if (acpi_debugger.ops) {
		ret = -EBUSY;
		goto err_lock;
	}

	acpi_debugger.owner = owner;
	acpi_debugger.ops = ops;

err_lock:
	mutex_unlock(&acpi_debugger.lock);
	return ret;
}