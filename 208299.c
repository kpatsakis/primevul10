acpi_os_predefined_override(const struct acpi_predefined_names *init_val,
			    char **new_val)
{
	if (!init_val || !new_val)
		return AE_BAD_PARAMETER;

	*new_val = NULL;
	if (!memcmp(init_val->name, "_OS_", 4) && strlen(acpi_os_name)) {
		printk(KERN_INFO PREFIX "Overriding _OS definition to '%s'\n",
		       acpi_os_name);
		*new_val = acpi_os_name;
	}

	if (!memcmp(init_val->name, "_REV", 4) && acpi_rev_override) {
		printk(KERN_INFO PREFIX "Overriding _REV return value to 5\n");
		*new_val = (char *)5;
	}

	return AE_OK;
}