static int ipmr_device_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct vif_device *v;
	int ct;
	if (event != NETDEV_UNREGISTER)
		return NOTIFY_DONE;
	v=&vif_table[0];
	for(ct=0;ct<maxvif;ct++,v++) {
		if (v->dev==ptr)
			vif_delete(ct);
	}
	return NOTIFY_DONE;
}