
static void sysfs_slab_remove_workfn(struct work_struct *work)
{
	struct kmem_cache *s =
		container_of(work, struct kmem_cache, kobj_remove_work);

	if (!s->kobj.state_in_sysfs)
		/*
		 * For a memcg cache, this may be called during
		 * deactivation and again on shutdown.  Remove only once.
		 * A cache is never shut down before deactivation is
		 * complete, so no need to worry about synchronization.
		 */
		goto out;

#ifdef CONFIG_MEMCG
	kset_unregister(s->memcg_kset);
#endif
	kobject_uevent(&s->kobj, KOBJ_REMOVE);
out:
	kobject_put(&s->kobj);