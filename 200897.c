static void __exit ibwdt_exit(void)
{
	platform_device_unregister(ibwdt_platform_device);
	platform_driver_unregister(&ibwdt_driver);
	printk(KERN_INFO PFX "Watchdog Module Unloaded.\n");
}