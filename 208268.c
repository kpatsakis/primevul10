acpi_os_install_interrupt_handler(u32 gsi, acpi_osd_handler handler,
				  void *context)
{
	unsigned int irq;

	acpi_irq_stats_init();

	/*
	 * ACPI interrupts different from the SCI in our copy of the FADT are
	 * not supported.
	 */
	if (gsi != acpi_gbl_FADT.sci_interrupt)
		return AE_BAD_PARAMETER;

	if (acpi_irq_handler)
		return AE_ALREADY_ACQUIRED;

	if (acpi_gsi_to_irq(gsi, &irq) < 0) {
		printk(KERN_ERR PREFIX "SCI (ACPI GSI %d) not registered\n",
		       gsi);
		return AE_OK;
	}

	acpi_irq_handler = handler;
	acpi_irq_context = context;
	if (request_irq(irq, acpi_irq, IRQF_SHARED, "acpi", acpi_irq)) {
		printk(KERN_ERR PREFIX "SCI (IRQ%d) allocation failed\n", irq);
		acpi_irq_handler = NULL;
		return AE_NOT_ACQUIRED;
	}
	acpi_sci_irq = irq;

	return AE_OK;
}