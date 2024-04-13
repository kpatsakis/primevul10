static inline struct iw_statistics *get_wireless_stats(struct net_device *dev)
{
	/* New location */
	if((dev->wireless_handlers != NULL) &&
	   (dev->wireless_handlers->get_wireless_stats != NULL))
		return dev->wireless_handlers->get_wireless_stats(dev);

	/* Old location, will be phased out in next WE */
	return (dev->get_wireless_stats ?
		dev->get_wireless_stats(dev) :
		(struct iw_statistics *) NULL);
}