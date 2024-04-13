walk_init_root(const char *name, struct nameidata *nd)
{
	struct fs_struct *fs = current->fs;

	read_lock(&fs->lock);
	if (fs->altroot.dentry && !(nd->flags & LOOKUP_NOALT)) {
		nd->path = fs->altroot;
		path_get(&fs->altroot);
		read_unlock(&fs->lock);
		if (__emul_lookup_dentry(name,nd))
			return 0;
		read_lock(&fs->lock);
	}
	nd->path = fs->root;
	path_get(&fs->root);
	read_unlock(&fs->lock);
	return 1;
}