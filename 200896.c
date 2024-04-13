static void ibwdt_disable(void)
{
	spin_lock(&ibwdt_lock);
	outb_p(0, WDT_STOP);
	spin_unlock(&ibwdt_lock);
}