static void __init resiliency_test(void)
{
	u8 *p;
	int type = KMALLOC_NORMAL;

	BUILD_BUG_ON(KMALLOC_MIN_SIZE > 16 || KMALLOC_SHIFT_HIGH < 10);

	pr_err("SLUB resiliency testing\n");
	pr_err("-----------------------\n");
	pr_err("A. Corruption after allocation\n");

	p = kzalloc(16, GFP_KERNEL);
	p[16] = 0x12;
	pr_err("\n1. kmalloc-16: Clobber Redzone/next pointer 0x12->0x%p\n\n",
	       p + 16);

	validate_slab_cache(kmalloc_caches[type][4]);

	/* Hmmm... The next two are dangerous */
	p = kzalloc(32, GFP_KERNEL);
	p[32 + sizeof(void *)] = 0x34;
	pr_err("\n2. kmalloc-32: Clobber next pointer/next slab 0x34 -> -0x%p\n",
	       p);
	pr_err("If allocated object is overwritten then not detectable\n\n");

	validate_slab_cache(kmalloc_caches[type][5]);
	p = kzalloc(64, GFP_KERNEL);
	p += 64 + (get_cycles() & 0xff) * sizeof(void *);
	*p = 0x56;
	pr_err("\n3. kmalloc-64: corrupting random byte 0x56->0x%p\n",
	       p);
	pr_err("If allocated object is overwritten then not detectable\n\n");
	validate_slab_cache(kmalloc_caches[type][6]);

	pr_err("\nB. Corruption after free\n");
	p = kzalloc(128, GFP_KERNEL);
	kfree(p);
	*p = 0x78;
	pr_err("1. kmalloc-128: Clobber first word 0x78->0x%p\n\n", p);
	validate_slab_cache(kmalloc_caches[type][7]);

	p = kzalloc(256, GFP_KERNEL);
	kfree(p);
	p[50] = 0x9a;
	pr_err("\n2. kmalloc-256: Clobber 50th byte 0x9a->0x%p\n\n", p);
	validate_slab_cache(kmalloc_caches[type][8]);

	p = kzalloc(512, GFP_KERNEL);
	kfree(p);
	p[512] = 0xab;
	pr_err("\n3. kmalloc-512: Clobber redzone 0xab->0x%p\n\n", p);
	validate_slab_cache(kmalloc_caches[type][9]);
}