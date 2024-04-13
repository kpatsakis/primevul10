int acpi_check_resource_conflict(const struct resource *res)
{
	acpi_adr_space_type space_id;
	acpi_size length;
	u8 warn = 0;
	int clash = 0;

	if (acpi_enforce_resources == ENFORCE_RESOURCES_NO)
		return 0;
	if (!(res->flags & IORESOURCE_IO) && !(res->flags & IORESOURCE_MEM))
		return 0;

	if (res->flags & IORESOURCE_IO)
		space_id = ACPI_ADR_SPACE_SYSTEM_IO;
	else
		space_id = ACPI_ADR_SPACE_SYSTEM_MEMORY;

	length = resource_size(res);
	if (acpi_enforce_resources != ENFORCE_RESOURCES_NO)
		warn = 1;
	clash = acpi_check_address_range(space_id, res->start, length, warn);

	if (clash) {
		if (acpi_enforce_resources != ENFORCE_RESOURCES_NO) {
			if (acpi_enforce_resources == ENFORCE_RESOURCES_LAX)
				printk(KERN_NOTICE "ACPI: This conflict may"
				       " cause random problems and system"
				       " instability\n");
			printk(KERN_INFO "ACPI: If an ACPI driver is available"
			       " for this device, you should use it instead of"
			       " the native driver\n");
		}
		if (acpi_enforce_resources == ENFORCE_RESOURCES_STRICT)
			return -EBUSY;
	}
	return 0;
}