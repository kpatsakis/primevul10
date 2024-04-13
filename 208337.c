acpi_status acpi_os_terminate(void)
{
	if (acpi_irq_handler) {
		acpi_os_remove_interrupt_handler(acpi_gbl_FADT.sci_interrupt,
						 acpi_irq_handler);
	}

	acpi_os_unmap_generic_address(&acpi_gbl_FADT.xgpe1_block);
	acpi_os_unmap_generic_address(&acpi_gbl_FADT.xgpe0_block);
	acpi_os_unmap_generic_address(&acpi_gbl_FADT.xpm1b_event_block);
	acpi_os_unmap_generic_address(&acpi_gbl_FADT.xpm1a_event_block);
	if (acpi_gbl_FADT.flags & ACPI_FADT_RESET_REGISTER)
		acpi_os_unmap_generic_address(&acpi_gbl_FADT.reset_register);

	destroy_workqueue(kacpid_wq);
	destroy_workqueue(kacpi_notify_wq);
	destroy_workqueue(kacpi_hotplug_wq);

	return AE_OK;
}