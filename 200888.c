static int ibwdt_close(struct inode *inode, struct file *file)
{
	if (expect_close == 42) {
		ibwdt_disable();
	} else {
		printk(KERN_CRIT PFX
		     "WDT device closed unexpectedly.  WDT will not stop!\n");
		ibwdt_ping();
	}
	clear_bit(0, &ibwdt_is_open);
	expect_close = 0;
	return 0;
}