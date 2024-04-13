void agp_generic_free_by_type(struct agp_memory *curr)
{
	agp_free_page_array(curr);
	agp_free_key(curr->key);
	kfree(curr);
}