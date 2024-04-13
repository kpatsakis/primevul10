static void acpi_os_map_cleanup(struct acpi_ioremap *map)
{
	if (!map->refcount) {
		synchronize_rcu_expedited();
		acpi_unmap(map->phys, map->virt);
		kfree(map);
	}
}