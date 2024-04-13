static void acpi_os_drop_map_ref(struct acpi_ioremap *map)
{
	if (!--map->refcount)
		list_del_rcu(&map->list);
}