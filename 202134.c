int addrconf_sysctl_forward(ctl_table *ctl, int write, struct file * filp,
			   void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int *valp = ctl->data;
	int val = *valp;
	int ret;

	ret = proc_dointvec(ctl, write, filp, buffer, lenp, ppos);

	if (write && valp != &ipv6_devconf_dflt.forwarding) {
		if (valp != &ipv6_devconf.forwarding) {
			if ((!*valp) ^ (!val)) {
				struct inet6_dev *idev = (struct inet6_dev *)ctl->extra1;
				if (idev == NULL)
					return ret;
				dev_forward_change(idev);
			}
		} else {
			ipv6_devconf_dflt.forwarding = ipv6_devconf.forwarding;
			addrconf_forward_change();
		}
		if (*valp)
			rt6_purge_dflt_routers();
	}

        return ret;
}