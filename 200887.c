static int __devinit ibwdt_probe(struct platform_device *dev)
{
	int res;

#if WDT_START != WDT_STOP
	if (!request_region(WDT_STOP, 1, "IB700 WDT")) {
		printk(KERN_ERR PFX "STOP method I/O %X is not available.\n",
								WDT_STOP);
		res = -EIO;
		goto out_nostopreg;
	}
#endif

	if (!request_region(WDT_START, 1, "IB700 WDT")) {
		printk(KERN_ERR PFX "START method I/O %X is not available.\n",
								WDT_START);
		res = -EIO;
		goto out_nostartreg;
	}

	res = misc_register(&ibwdt_miscdev);
	if (res) {
		printk(KERN_ERR PFX "failed to register misc device\n");
		goto out_nomisc;
	}
	return 0;

out_nomisc:
	release_region(WDT_START, 1);
out_nostartreg:
#if WDT_START != WDT_STOP
	release_region(WDT_STOP, 1);
#endif
out_nostopreg:
	return res;
}