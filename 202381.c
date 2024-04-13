void __init ip_mr_init(void)
{
	mrt_cachep = kmem_cache_create("ip_mrt_cache",
				       sizeof(struct mfc_cache),
				       0, SLAB_HWCACHE_ALIGN,
				       NULL, NULL);
	if (!mrt_cachep)
		panic("cannot allocate ip_mrt_cache");

	init_timer(&ipmr_expire_timer);
	ipmr_expire_timer.function=ipmr_expire_process;
	register_netdevice_notifier(&ip_mr_notifier);
#ifdef CONFIG_PROC_FS	
	proc_net_fops_create("ip_mr_vif", 0, &ipmr_vif_fops);
	proc_net_fops_create("ip_mr_cache", 0, &ipmr_mfc_fops);
#endif	
}