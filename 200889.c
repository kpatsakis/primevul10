static void ibwdt_ping(void)
{
	spin_lock(&ibwdt_lock);

	/* Write a watchdog value */
	outb_p(wd_margin, WDT_START);

	spin_unlock(&ibwdt_lock);
}