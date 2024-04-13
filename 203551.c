void b43warn(struct b43_wl *wl, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	if (b43_modparam_verbose < B43_VERBOSITY_WARN)
		return;
	if (!b43_ratelimit(wl))
		return;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	printk(KERN_WARNING "b43-%s warning: %pV",
	       (wl && wl->hw) ? wiphy_name(wl->hw->wiphy) : "wlan", &vaf);

	va_end(args);
}