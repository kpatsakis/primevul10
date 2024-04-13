clusterip_config_entry_put(struct clusterip_config *c)
{
	local_bh_disable();
	if (atomic_dec_and_lock(&c->entries, &clusterip_lock)) {
		list_del_rcu(&c->list);
		spin_unlock(&clusterip_lock);
		local_bh_enable();

		dev_mc_del(c->dev, c->clustermac);
		dev_put(c->dev);

		/* In case anyone still accesses the file, the open/close
		 * functions are also incrementing the refcount on their own,
		 * so it's safe to remove the entry even if it's in use. */
#ifdef CONFIG_PROC_FS
		remove_proc_entry(c->pde->name, c->pde->parent);
#endif
		return;
	}
	local_bh_enable();
}