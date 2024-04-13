inotify_dev_get_last_event(struct inotify_device *dev)
{
	if (list_empty(&dev->events))
		return NULL;
	return list_entry(dev->events.prev, struct inotify_kernel_event, list);
}