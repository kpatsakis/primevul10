static int vif_add(struct vifctl *vifc, int mrtsock)
{
	int vifi = vifc->vifc_vifi;
	struct vif_device *v = &vif_table[vifi];
	struct net_device *dev;
	struct in_device *in_dev;

	/* Is vif busy ? */
	if (VIF_EXISTS(vifi))
		return -EADDRINUSE;

	switch (vifc->vifc_flags) {
#ifdef CONFIG_IP_PIMSM
	case VIFF_REGISTER:
		/*
		 * Special Purpose VIF in PIM
		 * All the packets will be sent to the daemon
		 */
		if (reg_vif_num >= 0)
			return -EADDRINUSE;
		dev = ipmr_reg_vif();
		if (!dev)
			return -ENOBUFS;
		break;
#endif
	case VIFF_TUNNEL:	
		dev = ipmr_new_tunnel(vifc);
		if (!dev)
			return -ENOBUFS;
		break;
	case 0:
		dev=ip_dev_find(vifc->vifc_lcl_addr.s_addr);
		if (!dev)
			return -EADDRNOTAVAIL;
		__dev_put(dev);
		break;
	default:
		return -EINVAL;
	}

	if ((in_dev = __in_dev_get(dev)) == NULL)
		return -EADDRNOTAVAIL;
	in_dev->cnf.mc_forwarding++;
	dev_set_allmulti(dev, +1);
	ip_rt_multicast_event(in_dev);

	/*
	 *	Fill in the VIF structures
	 */
	v->rate_limit=vifc->vifc_rate_limit;
	v->local=vifc->vifc_lcl_addr.s_addr;
	v->remote=vifc->vifc_rmt_addr.s_addr;
	v->flags=vifc->vifc_flags;
	if (!mrtsock)
		v->flags |= VIFF_STATIC;
	v->threshold=vifc->vifc_threshold;
	v->bytes_in = 0;
	v->bytes_out = 0;
	v->pkt_in = 0;
	v->pkt_out = 0;
	v->link = dev->ifindex;
	if (v->flags&(VIFF_TUNNEL|VIFF_REGISTER))
		v->link = dev->iflink;

	/* And finish update writing critical data */
	write_lock_bh(&mrt_lock);
	dev_hold(dev);
	v->dev=dev;
#ifdef CONFIG_IP_PIMSM
	if (v->flags&VIFF_REGISTER)
		reg_vif_num = vifi;
#endif
	if (vifi+1 > maxvif)
		maxvif = vifi+1;
	write_unlock_bh(&mrt_lock);
	return 0;
}