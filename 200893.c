static long ibwdt_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int new_margin;
	void __user *argp = (void __user *)arg;
	int __user *p = argp;

	static struct watchdog_info ident = {
		.options = WDIOF_KEEPALIVEPING | WDIOF_SETTIMEOUT
							| WDIOF_MAGICCLOSE,
		.firmware_version = 1,
		.identity = "IB700 WDT",
	};

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		if (copy_to_user(argp, &ident, sizeof(ident)))
			return -EFAULT;
		break;

	case WDIOC_GETSTATUS:
	case WDIOC_GETBOOTSTATUS:
		return put_user(0, p);

	case WDIOC_SETOPTIONS:
	{
		int options, retval = -EINVAL;

		if (get_user(options, p))
			return -EFAULT;

		if (options & WDIOS_DISABLECARD) {
			ibwdt_disable();
			retval = 0;
		}
		if (options & WDIOS_ENABLECARD) {
			ibwdt_ping();
			retval = 0;
		}
		return retval;
	}
	case WDIOC_KEEPALIVE:
		ibwdt_ping();
		break;

	case WDIOC_SETTIMEOUT:
		if (get_user(new_margin, p))
			return -EFAULT;
		if (ibwdt_set_heartbeat(new_margin))
			return -EINVAL;
		ibwdt_ping();
		/* Fall */

	case WDIOC_GETTIMEOUT:
		return put_user(wd_times[wd_margin], p);

	default:
		return -ENOTTY;
	}
	return 0;
}