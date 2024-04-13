static inline int ioctl_standard_call(struct net_device *	dev,
				      struct ifreq *		ifr,
				      unsigned int		cmd,
				      iw_handler		handler)
{
	struct iwreq *				iwr = (struct iwreq *) ifr;
	const struct iw_ioctl_description *	descr;
	struct iw_request_info			info;
	int					ret = -EINVAL;

	/* Get the description of the IOCTL */
	if((cmd - SIOCIWFIRST) >= standard_ioctl_num)
		return -EOPNOTSUPP;
	descr = &(standard_ioctl[cmd - SIOCIWFIRST]);

#ifdef WE_IOCTL_DEBUG
	printk(KERN_DEBUG "%s (WE) : Found standard handler for 0x%04X\n",
	       ifr->ifr_name, cmd);
	printk(KERN_DEBUG "%s (WE) : Header type : %d, Token type : %d, size : %d, token : %d\n", dev->name, descr->header_type, descr->token_type, descr->token_size, descr->max_tokens);
#endif	/* WE_IOCTL_DEBUG */

	/* Prepare the call */
	info.cmd = cmd;
	info.flags = 0;

	/* Check if we have a pointer to user space data or not */
	if(descr->header_type != IW_HEADER_TYPE_POINT) {

		/* No extra arguments. Trivial to handle */
		ret = handler(dev, &info, &(iwr->u), NULL);

#ifdef WE_SET_EVENT
		/* Generate an event to notify listeners of the change */
		if((descr->flags & IW_DESCR_FLAG_EVENT) &&
		   ((ret == 0) || (ret == -EIWCOMMIT)))
			wireless_send_event(dev, cmd, &(iwr->u), NULL);
#endif	/* WE_SET_EVENT */
	} else {
		char *	extra;
		int	extra_size;
		int	user_length = 0;
		int	err;

		/* Calculate space needed by arguments. Always allocate
		 * for max space. Easier, and won't last long... */
		extra_size = descr->max_tokens * descr->token_size;

		/* Check what user space is giving us */
		if(IW_IS_SET(cmd)) {
			/* Check NULL pointer */
			if((iwr->u.data.pointer == NULL) &&
			   (iwr->u.data.length != 0))
				return -EFAULT;
			/* Check if number of token fits within bounds */
			if(iwr->u.data.length > descr->max_tokens)
				return -E2BIG;
			if(iwr->u.data.length < descr->min_tokens)
				return -EINVAL;
		} else {
			/* Check NULL pointer */
			if(iwr->u.data.pointer == NULL)
				return -EFAULT;
			/* Save user space buffer size for checking */
			user_length = iwr->u.data.length;

			/* Don't check if user_length > max to allow forward
			 * compatibility. The test user_length < min is
			 * implied by the test at the end. */

			/* Support for very large requests */
			if((descr->flags & IW_DESCR_FLAG_NOMAX) &&
			   (user_length > descr->max_tokens)) {
				/* Allow userspace to GET more than max so
				 * we can support any size GET requests.
				 * There is still a limit : -ENOMEM. */
				extra_size = user_length * descr->token_size;
				/* Note : user_length is originally a __u16,
				 * and token_size is controlled by us,
				 * so extra_size won't get negative and
				 * won't overflow... */
			}
		}

#ifdef WE_IOCTL_DEBUG
		printk(KERN_DEBUG "%s (WE) : Malloc %d bytes\n",
		       dev->name, extra_size);
#endif	/* WE_IOCTL_DEBUG */

		/* Create the kernel buffer */
		extra = kmalloc(extra_size, GFP_KERNEL);
		if (extra == NULL) {
			return -ENOMEM;
		}

		/* If it is a SET, get all the extra data in here */
		if(IW_IS_SET(cmd) && (iwr->u.data.length != 0)) {
			err = copy_from_user(extra, iwr->u.data.pointer,
					     iwr->u.data.length *
					     descr->token_size);
			if (err) {
				kfree(extra);
				return -EFAULT;
			}
#ifdef WE_IOCTL_DEBUG
			printk(KERN_DEBUG "%s (WE) : Got %d bytes\n",
			       dev->name,
			       iwr->u.data.length * descr->token_size);
#endif	/* WE_IOCTL_DEBUG */
		}

		/* Call the handler */
		ret = handler(dev, &info, &(iwr->u), extra);

		/* If we have something to return to the user */
		if (!ret && IW_IS_GET(cmd)) {
			/* Check if there is enough buffer up there */
			if(user_length < iwr->u.data.length) {
				kfree(extra);
				return -E2BIG;
			}

			err = copy_to_user(iwr->u.data.pointer, extra,
					   iwr->u.data.length *
					   descr->token_size);
			if (err)
				ret =  -EFAULT;				   
#ifdef WE_IOCTL_DEBUG
			printk(KERN_DEBUG "%s (WE) : Wrote %d bytes\n",
			       dev->name,
			       iwr->u.data.length * descr->token_size);
#endif	/* WE_IOCTL_DEBUG */
		}

#ifdef WE_SET_EVENT
		/* Generate an event to notify listeners of the change */
		if((descr->flags & IW_DESCR_FLAG_EVENT) &&
		   ((ret == 0) || (ret == -EIWCOMMIT))) {
			if(descr->flags & IW_DESCR_FLAG_RESTRICT)
				/* If the event is restricted, don't
				 * export the payload */
				wireless_send_event(dev, cmd, &(iwr->u), NULL);
			else
				wireless_send_event(dev, cmd, &(iwr->u),
						    extra);
		}
#endif	/* WE_SET_EVENT */

		/* Cleanup - I told you it wasn't that long ;-) */
		kfree(extra);
	}

	/* Call commit handler if needed and defined */
	if(ret == -EIWCOMMIT)
		ret = call_commit_handler(dev);

	/* Here, we will generate the appropriate event if needed */

	return ret;
}