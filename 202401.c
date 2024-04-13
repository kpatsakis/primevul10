static inline iw_handler get_handler(struct net_device *dev,
				     unsigned int cmd)
{
	/* Don't "optimise" the following variable, it will crash */
	unsigned int	index;		/* *MUST* be unsigned */

	/* Check if we have some wireless handlers defined */
	if(dev->wireless_handlers == NULL)
		return NULL;

	/* Try as a standard command */
	index = cmd - SIOCIWFIRST;
	if(index < dev->wireless_handlers->num_standard)
		return dev->wireless_handlers->standard[index];

	/* Try as a private command */
	index = cmd - SIOCIWFIRSTPRIV;
	if(index < dev->wireless_handlers->num_private)
		return dev->wireless_handlers->private[index];

	/* Not found */
	return NULL;
}