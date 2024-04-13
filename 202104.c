static int addrconf_sysctl_forward_strategy(ctl_table *table, 
					    int __user *name, int nlen,
					    void __user *oldval,
					    size_t __user *oldlenp,
					    void __user *newval, size_t newlen,
					    void **context)
{
	int *valp = table->data;
	int new;

	if (!newval || !newlen)
		return 0;
	if (newlen != sizeof(int))
		return -EINVAL;
	if (get_user(new, (int __user *)newval))
		return -EFAULT;
	if (new == *valp)
		return 0;
	if (oldval && oldlenp) {
		size_t len;
		if (get_user(len, oldlenp))
			return -EFAULT;
		if (len) {
			if (len > table->maxlen)
				len = table->maxlen;
			if (copy_to_user(oldval, valp, len))
				return -EFAULT;
			if (put_user(len, oldlenp))
				return -EFAULT;
		}
	}

	if (valp != &ipv6_devconf_dflt.forwarding) {
		if (valp != &ipv6_devconf.forwarding) {
			struct inet6_dev *idev = (struct inet6_dev *)table->extra1;
			int changed;
			if (unlikely(idev == NULL))
				return -ENODEV;
			changed = (!*valp) ^ (!new);
			*valp = new;
			if (changed)
				dev_forward_change(idev);
		} else {
			*valp = new;
			addrconf_forward_change();
		}

		if (*valp)
			rt6_purge_dflt_routers();
	} else
		*valp = new;

	return 1;
}