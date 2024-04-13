static void put_map(unsigned long *map)
{
	VM_BUG_ON(map != object_map);
	lockdep_assert_held(&object_map_lock);

	spin_unlock(&object_map_lock);
}