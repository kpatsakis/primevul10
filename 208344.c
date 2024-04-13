acpi_status acpi_os_remove_interrupt_handler(u32 gsi, acpi_osd_handler handler)
{
	if (gsi != acpi_gbl_FADT.sci_interrupt || !acpi_sci_irq_valid())
		return AE_BAD_PARAMETER;

	free_irq(acpi_sci_irq, acpi_irq);
	acpi_irq_handler = NULL;
	acpi_sci_irq = INVALID_ACPI_IRQ;

	return AE_OK;
}