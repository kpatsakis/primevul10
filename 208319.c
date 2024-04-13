acpi_status __init acpi_os_initialize1(void)
{
	kacpid_wq = alloc_workqueue("kacpid", 0, 1);
	kacpi_notify_wq = alloc_workqueue("kacpi_notify", 0, 1);
	kacpi_hotplug_wq = alloc_ordered_workqueue("kacpi_hotplug", 0);
	BUG_ON(!kacpid_wq);
	BUG_ON(!kacpi_notify_wq);
	BUG_ON(!kacpi_hotplug_wq);
	acpi_install_interface_handler(acpi_osi_handler);
	acpi_osi_setup_late();
	return AE_OK;
}