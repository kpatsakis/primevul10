int __init addrconf_init(void)
{
	int err = 0;

	/* The addrconf netdev notifier requires that loopback_dev
	 * has it's ipv6 private information allocated and setup
	 * before it can bring up and give link-local addresses
	 * to other devices which are up.
	 *
	 * Unfortunately, loopback_dev is not necessarily the first
	 * entry in the global dev_base list of net devices.  In fact,
	 * it is likely to be the very last entry on that list.
	 * So this causes the notifier registry below to try and
	 * give link-local addresses to all devices besides loopback_dev
	 * first, then loopback_dev, which cases all the non-loopback_dev
	 * devices to fail to get a link-local address.
	 *
	 * So, as a temporary fix, allocate the ipv6 structure for
	 * loopback_dev first by hand.
	 * Longer term, all of the dependencies ipv6 has upon the loopback
	 * device and it being up should be removed.
	 */
	rtnl_lock();
	if (!ipv6_add_dev(&loopback_dev))
		err = -ENOMEM;
	rtnl_unlock();
	if (err)
		return err;

	register_netdevice_notifier(&ipv6_dev_notf);

#ifdef CONFIG_IPV6_PRIVACY
	md5_tfm = crypto_alloc_tfm("md5", 0);
	if (unlikely(md5_tfm == NULL))
		printk(KERN_WARNING
			"failed to load transform for md5\n");
#endif

	addrconf_verify(0);
	rtnetlink_links[PF_INET6] = inet6_rtnetlink_table;
#ifdef CONFIG_SYSCTL
	addrconf_sysctl.sysctl_header =
		register_sysctl_table(addrconf_sysctl.addrconf_root_dir, 0);
	addrconf_sysctl_register(NULL, &ipv6_devconf_dflt);
#endif

	return 0;
}