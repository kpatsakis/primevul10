static void iw_send_thrspy_event(struct net_device *	dev,
				 struct iw_spy_data *	spydata,
				 unsigned char *	address,
				 struct iw_quality *	wstats)
{
	union iwreq_data	wrqu;
	struct iw_thrspy	threshold;

	/* Init */
	wrqu.data.length = 1;
	wrqu.data.flags = 0;
	/* Copy address */
	memcpy(threshold.addr.sa_data, address, ETH_ALEN);
	threshold.addr.sa_family = ARPHRD_ETHER;
	/* Copy stats */
	memcpy(&(threshold.qual), wstats, sizeof(struct iw_quality));
	/* Copy also thresholds */
	memcpy(&(threshold.low), &(spydata->spy_thr_low),
	       2 * sizeof(struct iw_quality));

#ifdef WE_SPY_DEBUG
	printk(KERN_DEBUG "iw_send_thrspy_event() : address %02X:%02X:%02X:%02X:%02X:%02X, level %d, up = %d\n",
	       threshold.addr.sa_data[0],
	       threshold.addr.sa_data[1],
	       threshold.addr.sa_data[2],
	       threshold.addr.sa_data[3],
	       threshold.addr.sa_data[4],
	       threshold.addr.sa_data[5], threshold.qual.level);
#endif	/* WE_SPY_DEBUG */

	/* Send event to user space */
	wireless_send_event(dev, SIOCGIWTHRSPY, &wrqu, (char *) &threshold);
}