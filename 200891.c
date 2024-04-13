static int __devexit ibwdt_remove(struct platform_device *dev)
{
	misc_deregister(&ibwdt_miscdev);
	release_region(WDT_START, 1);
#if WDT_START != WDT_STOP
	release_region(WDT_STOP, 1);
#endif
	return 0;
}