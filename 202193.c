static void ipv6_regen_rndid(unsigned long data)
{
	struct inet6_dev *idev = (struct inet6_dev *) data;
	unsigned long expires;

	read_lock_bh(&addrconf_lock);
	write_lock_bh(&idev->lock);

	if (idev->dead)
		goto out;

	if (__ipv6_regen_rndid(idev) < 0)
		goto out;
	
	expires = jiffies +
		idev->cnf.temp_prefered_lft * HZ - 
		idev->cnf.regen_max_retry * idev->cnf.dad_transmits * idev->nd_parms->retrans_time - desync_factor;
	if (time_before(expires, jiffies)) {
		printk(KERN_WARNING
			"ipv6_regen_rndid(): too short regeneration interval; timer disabled for %s.\n",
			idev->dev->name);
		goto out;
	}

	if (!mod_timer(&idev->regen_timer, expires))
		in6_dev_hold(idev);

out:
	write_unlock_bh(&idev->lock);
	read_unlock_bh(&addrconf_lock);
	in6_dev_put(idev);
}