static int __init ibwdt_init(void)
{
	int err;

	printk(KERN_INFO PFX
		"WDT driver for IB700 single board computer initialising.\n");

	err = platform_driver_register(&ibwdt_driver);
	if (err)
		return err;

	ibwdt_platform_device = platform_device_register_simple(DRV_NAME,
								-1, NULL, 0);
	if (IS_ERR(ibwdt_platform_device)) {
		err = PTR_ERR(ibwdt_platform_device);
		goto unreg_platform_driver;
	}

	return 0;

unreg_platform_driver:
	platform_driver_unregister(&ibwdt_driver);
	return err;
}