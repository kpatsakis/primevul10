static int path_walk(const char *name, struct nameidata *nd)
{
	current->total_link_count = 0;
	return link_path_walk(name, nd);
}