int wireless_process_ioctl(struct ifreq *ifr, unsigned int cmd)
{
	struct net_device *dev;
	iw_handler	handler;

	/* Permissions are already checked in dev_ioctl() before calling us.
	 * The copy_to/from_user() of ifr is also dealt with in there */

	/* Make sure the device exist */
	if ((dev = __dev_get_by_name(ifr->ifr_name)) == NULL)
		return -ENODEV;

	/* A bunch of special cases, then the generic case...
	 * Note that 'cmd' is already filtered in dev_ioctl() with
	 * (cmd >= SIOCIWFIRST && cmd <= SIOCIWLAST) */
	switch(cmd) 
	{
		case SIOCGIWSTATS:
			/* Get Wireless Stats */
			return dev_iwstats(dev, ifr);

		case SIOCGIWPRIV:
			/* Check if we have some wireless handlers defined */
			if(dev->wireless_handlers != NULL) {
				/* We export to user space the definition of
				 * the private handler ourselves */
				return ioctl_export_private(dev, ifr);
			}
			// ## Fall-through for old API ##
		default:
			/* Generic IOCTL */
			/* Basic check */
			if (!netif_device_present(dev))
				return -ENODEV;
			/* New driver API : try to find the handler */
			handler = get_handler(dev, cmd);
			if(handler != NULL) {
				/* Standard and private are not the same */
				if(cmd < SIOCIWFIRSTPRIV)
					return ioctl_standard_call(dev,
								   ifr,
								   cmd,
								   handler);
				else
					return ioctl_private_call(dev,
								  ifr,
								  cmd,
								  handler);
			}
			/* Old driver API : call driver ioctl handler */
			if (dev->do_ioctl) {
				return dev->do_ioctl(dev, ifr, cmd);
			}
			return -EOPNOTSUPP;
	}
	/* Not reached */
	return -EINVAL;
}