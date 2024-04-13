lynx_release(struct kref *kref)
{
	kfree(container_of(kref, struct pcilynx, kref));
}