acpi_os_map_iomem(acpi_physical_address phys, acpi_size size)
{
	struct acpi_ioremap *map;
	void __iomem *virt;
	acpi_physical_address pg_off;
	acpi_size pg_sz;

	if (phys > ULONG_MAX) {
		printk(KERN_ERR PREFIX "Cannot map memory that high\n");
		return NULL;
	}

	if (!acpi_gbl_permanent_mmap)
		return __acpi_map_table((unsigned long)phys, size);

	mutex_lock(&acpi_ioremap_lock);
	/* Check if there's a suitable mapping already. */
	map = acpi_map_lookup(phys, size);
	if (map) {
		map->refcount++;
		goto out;
	}

	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (!map) {
		mutex_unlock(&acpi_ioremap_lock);
		return NULL;
	}

	pg_off = round_down(phys, PAGE_SIZE);
	pg_sz = round_up(phys + size, PAGE_SIZE) - pg_off;
	virt = acpi_map(pg_off, pg_sz);
	if (!virt) {
		mutex_unlock(&acpi_ioremap_lock);
		kfree(map);
		return NULL;
	}

	INIT_LIST_HEAD(&map->list);
	map->virt = virt;
	map->phys = pg_off;
	map->size = pg_sz;
	map->refcount = 1;

	list_add_tail_rcu(&map->list, &acpi_ioremaps);

out:
	mutex_unlock(&acpi_ioremap_lock);
	return map->virt + (phys - map->phys);
}