static int ipmr_find_vif(struct net_device *dev)
{
	int ct;
	for (ct=maxvif-1; ct>=0; ct--) {
		if (vif_table[ct].dev == dev)
			break;
	}
	return ct;
}