static inline struct iw_spy_data * get_spydata(struct net_device *dev)
{
	/* This is the new way */
	if(dev->wireless_data)
		return(dev->wireless_data->spy_data);

	/* This is the old way. Doesn't work for multi-headed drivers.
	 * It will be removed in the next version of WE. */
	return (dev->priv + dev->wireless_handlers->spy_offset);
}