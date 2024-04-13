inotify_dev_get_event(struct inotify_device *dev)
{
	return list_entry(dev->events.next, struct inotify_kernel_event, list);
}