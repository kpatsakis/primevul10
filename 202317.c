static void ipmr_expire_process(unsigned long dummy)
{
	unsigned long now;
	unsigned long expires;
	struct mfc_cache *c, **cp;

	if (!spin_trylock(&mfc_unres_lock)) {
		mod_timer(&ipmr_expire_timer, jiffies+HZ/10);
		return;
	}

	if (atomic_read(&cache_resolve_queue_len) == 0)
		goto out;

	now = jiffies;
	expires = 10*HZ;
	cp = &mfc_unres_queue;

	while ((c=*cp) != NULL) {
		if (time_after(c->mfc_un.unres.expires, now)) {
			unsigned long interval = c->mfc_un.unres.expires - now;
			if (interval < expires)
				expires = interval;
			cp = &c->next;
			continue;
		}

		*cp = c->next;

		ipmr_destroy_unres(c);
	}

	if (atomic_read(&cache_resolve_queue_len))
		mod_timer(&ipmr_expire_timer, jiffies + expires);

out:
	spin_unlock(&mfc_unres_lock);
}