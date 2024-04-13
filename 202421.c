static inline int ioctl_export_private(struct net_device *	dev,
				       struct ifreq *		ifr)
{
	struct iwreq *				iwr = (struct iwreq *) ifr;

	/* Check if the driver has something to export */
	if((dev->wireless_handlers->num_private_args == 0) ||
	   (dev->wireless_handlers->private_args == NULL))
		return -EOPNOTSUPP;

	/* Check NULL pointer */
	if(iwr->u.data.pointer == NULL)
		return -EFAULT;

	/* Check if there is enough buffer up there */
	if(iwr->u.data.length < dev->wireless_handlers->num_private_args) {
		/* User space can't know in advance how large the buffer
		 * needs to be. Give it a hint, so that we can support
		 * any size buffer we want somewhat efficiently... */
		iwr->u.data.length = dev->wireless_handlers->num_private_args;
		return -E2BIG;
	}

	/* Set the number of available ioctls. */
	iwr->u.data.length = dev->wireless_handlers->num_private_args;

	/* Copy structure to the user buffer. */
	if (copy_to_user(iwr->u.data.pointer,
			 dev->wireless_handlers->private_args,
			 sizeof(struct iw_priv_args) * iwr->u.data.length))
		return -EFAULT;

	return 0;
}