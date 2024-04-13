static inline void __flush_cpu_slab(struct kmem_cache *s, int cpu)
{
	struct kmem_cache_cpu *c = per_cpu_ptr(s->cpu_slab, cpu);

	if (c->page)
		flush_slab(s, c);

	unfreeze_partials(s, c);
}