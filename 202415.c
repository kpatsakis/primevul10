static inline int ioctl_private_call(struct net_device *	dev,
				     struct ifreq *		ifr,
				     unsigned int		cmd,
				     iw_handler		handler)
{
	struct iwreq *			iwr = (struct iwreq *) ifr;
	const struct iw_priv_args *	descr = NULL;
	struct iw_request_info		info;
	int				extra_size = 0;
	int				i;
	int				ret = -EINVAL;

	/* Get the description of the IOCTL */
	for(i = 0; i < dev->wireless_handlers->num_private_args; i++)
		if(cmd == dev->wireless_handlers->private_args[i].cmd) {
			descr = &(dev->wireless_handlers->private_args[i]);
			break;
		}

#ifdef WE_IOCTL_DEBUG
	printk(KERN_DEBUG "%s (WE) : Found private handler for 0x%04X\n",
	       ifr->ifr_name, cmd);
	if(descr) {
		printk(KERN_DEBUG "%s (WE) : Name %s, set %X, get %X\n",
		       dev->name, descr->name,
		       descr->set_args, descr->get_args);
	}
#endif	/* WE_IOCTL_DEBUG */

	/* Compute the size of the set/get arguments */
	if(descr != NULL) {
		if(IW_IS_SET(cmd)) {
			int	offset = 0;	/* For sub-ioctls */
			/* Check for sub-ioctl handler */
			if(descr->name[0] == '\0')
				/* Reserve one int for sub-ioctl index */
				offset = sizeof(__u32);

			/* Size of set arguments */
			extra_size = get_priv_size(descr->set_args);

			/* Does it fits in iwr ? */
			if((descr->set_args & IW_PRIV_SIZE_FIXED) &&
			   ((extra_size + offset) <= IFNAMSIZ))
				extra_size = 0;
		} else {
			/* Size of get arguments */
			extra_size = get_priv_size(descr->get_args);

			/* Does it fits in iwr ? */
			if((descr->get_args & IW_PRIV_SIZE_FIXED) &&
			   (extra_size <= IFNAMSIZ))
				extra_size = 0;
		}
	}

	/* Prepare the call */
	info.cmd = cmd;
	info.flags = 0;

	/* Check if we have a pointer to user space data or not. */
	if(extra_size == 0) {
		/* No extra arguments. Trivial to handle */
		ret = handler(dev, &info, &(iwr->u), (char *) &(iwr->u));
	} else {
		char *	extra;
		int	err;

		/* Check what user space is giving us */
		if(IW_IS_SET(cmd)) {
			/* Check NULL pointer */
			if((iwr->u.data.pointer == NULL) &&
			   (iwr->u.data.length != 0))
				return -EFAULT;

			/* Does it fits within bounds ? */
			if(iwr->u.data.length > (descr->set_args &
						 IW_PRIV_SIZE_MASK))
				return -E2BIG;
		} else {
			/* Check NULL pointer */
			if(iwr->u.data.pointer == NULL)
				return -EFAULT;
		}

#ifdef WE_IOCTL_DEBUG
		printk(KERN_DEBUG "%s (WE) : Malloc %d bytes\n",
		       dev->name, extra_size);
#endif	/* WE_IOCTL_DEBUG */

		/* Always allocate for max space. Easier, and won't last
		 * long... */
		extra = kmalloc(extra_size, GFP_KERNEL);
		if (extra == NULL) {
			return -ENOMEM;
		}

		/* If it is a SET, get all the extra data in here */
		if(IW_IS_SET(cmd) && (iwr->u.data.length != 0)) {
			err = copy_from_user(extra, iwr->u.data.pointer,
					     extra_size);
			if (err) {
				kfree(extra);
				return -EFAULT;
			}
#ifdef WE_IOCTL_DEBUG
			printk(KERN_DEBUG "%s (WE) : Got %d elem\n",
			       dev->name, iwr->u.data.length);
#endif	/* WE_IOCTL_DEBUG */
		}

		/* Call the handler */
		ret = handler(dev, &info, &(iwr->u), extra);

		/* If we have something to return to the user */
		if (!ret && IW_IS_GET(cmd)) {

			/* Adjust for the actual length if it's variable,
			 * avoid leaking kernel bits outside. */
			if (!(descr->get_args & IW_PRIV_SIZE_FIXED)) {
				extra_size = adjust_priv_size(descr->get_args,
							      &(iwr->u));
			}

			err = copy_to_user(iwr->u.data.pointer, extra,
					   extra_size);
			if (err)
				ret =  -EFAULT;				   
#ifdef WE_IOCTL_DEBUG
			printk(KERN_DEBUG "%s (WE) : Wrote %d elem\n",
			       dev->name, iwr->u.data.length);
#endif	/* WE_IOCTL_DEBUG */
		}

		/* Cleanup - I told you it wasn't that long ;-) */
		kfree(extra);
	}


	/* Call commit handler if needed and defined */
	if(ret == -EIWCOMMIT)
		ret = call_commit_handler(dev);

	return ret;
}