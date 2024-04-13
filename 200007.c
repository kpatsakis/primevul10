isdn_read(struct file *file, char __user *buf, size_t count, loff_t * off)
{
	uint minor = iminor(file->f_path.dentry->d_inode);
	int len = 0;
	int drvidx;
	int chidx;
	int retval;
	char *p;

	lock_kernel();
	if (minor == ISDN_MINOR_STATUS) {
		if (!file->private_data) {
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				goto out;
			}
			interruptible_sleep_on(&(dev->info_waitq));
		}
		p = isdn_statstr();
		file->private_data = NULL;
		if ((len = strlen(p)) <= count) {
			if (copy_to_user(buf, p, len)) {
				retval = -EFAULT;
				goto out;
			}
			*off += len;
			retval = len;
			goto out;
		}
		retval = 0;
		goto out;
	}
	if (!dev->drivers) {
		retval = -ENODEV;
		goto out;
	}
	if (minor <= ISDN_MINOR_BMAX) {
		printk(KERN_WARNING "isdn_read minor %d obsolete!\n", minor);
		drvidx = isdn_minor2drv(minor);
		if (drvidx < 0) {
			retval = -ENODEV;
			goto out;
		}
		if (!(dev->drv[drvidx]->flags & DRV_FLAG_RUNNING)) {
			retval = -ENODEV;
			goto out;
		}
		chidx = isdn_minor2chan(minor);
		if (!(p = kmalloc(count, GFP_KERNEL))) {
			retval = -ENOMEM;
			goto out;
		}
		len = isdn_readbchan(drvidx, chidx, p, NULL, count,
				     &dev->drv[drvidx]->rcv_waitq[chidx]);
		*off += len;
		if (copy_to_user(buf,p,len)) 
			len = -EFAULT;
		kfree(p);
		retval = len;
		goto out;
	}
	if (minor <= ISDN_MINOR_CTRLMAX) {
		drvidx = isdn_minor2drv(minor - ISDN_MINOR_CTRL);
		if (drvidx < 0) {
			retval = -ENODEV;
			goto out;
		}
		if (!dev->drv[drvidx]->stavail) {
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				goto out;
			}
			interruptible_sleep_on(&(dev->drv[drvidx]->st_waitq));
		}
		if (dev->drv[drvidx]->interface->readstat) {
			if (count > dev->drv[drvidx]->stavail)
				count = dev->drv[drvidx]->stavail;
			len = dev->drv[drvidx]->interface->readstat(buf, count,
				drvidx, isdn_minor2chan(minor - ISDN_MINOR_CTRL));
			if (len < 0) {
				retval = len;
				goto out;
			}
		} else {
			len = 0;
		}
		if (len)
			dev->drv[drvidx]->stavail -= len;
		else
			dev->drv[drvidx]->stavail = 0;
		*off += len;
		retval = len;
		goto out;
	}
#ifdef CONFIG_ISDN_PPP
	if (minor <= ISDN_MINOR_PPPMAX) {
		retval = isdn_ppp_read(minor - ISDN_MINOR_PPP, file, buf, count);
		goto out;
	}
#endif
	retval = -ENODEV;
 out:
	unlock_kernel();
	return retval;
}