static int slub_cpu_dead(unsigned int cpu)
{
	struct kmem_cache *s;
	unsigned long flags;

	mutex_lock(&slab_mutex);
	list_for_each_entry(s, &slab_caches, list) {
		local_irq_save(flags);
		__flush_cpu_slab(s, cpu);
		local_irq_restore(flags);
	}
	mutex_unlock(&slab_mutex);
	return 0;
}