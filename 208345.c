bool acpi_queue_hotplug_work(struct work_struct *work)
{
	return queue_work(kacpi_hotplug_wq, work);
}