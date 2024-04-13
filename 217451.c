void __kmemcg_cache_deactivate_after_rcu(struct kmem_cache *s)
{
	/*
	 * Called with all the locks held after a sched RCU grace period.
	 * Even if @s becomes empty after shrinking, we can't know that @s
	 * doesn't have allocations already in-flight and thus can't
	 * destroy @s until the associated memcg is released.
	 *
	 * However, let's remove the sysfs files for empty caches here.
	 * Each cache has a lot of interface files which aren't
	 * particularly useful for empty draining caches; otherwise, we can
	 * easily end up with millions of unnecessary sysfs files on
	 * systems which have a lot of memory and transient cgroups.
	 */
	if (!__kmem_cache_shrink(s))
		sysfs_slab_remove(s);
}