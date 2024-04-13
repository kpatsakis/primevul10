static struct net_device_stats *reg_vif_get_stats(struct net_device *dev)
{
	return (struct net_device_stats*)dev->priv;
}