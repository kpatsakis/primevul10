static int __emul_lookup_dentry(const char *name, struct nameidata *nd)
{
	if (path_walk(name, nd))
		return 0;		/* something went wrong... */

	if (!nd->path.dentry->d_inode ||
	    S_ISDIR(nd->path.dentry->d_inode->i_mode)) {
		struct path old_path = nd->path;
		struct qstr last = nd->last;
		int last_type = nd->last_type;
		struct fs_struct *fs = current->fs;

		/*
		 * NAME was not found in alternate root or it's a directory.
		 * Try to find it in the normal root:
		 */
		nd->last_type = LAST_ROOT;
		read_lock(&fs->lock);
		nd->path = fs->root;
		path_get(&fs->root);
		read_unlock(&fs->lock);
		if (path_walk(name, nd) == 0) {
			if (nd->path.dentry->d_inode) {
				path_put(&old_path);
				return 1;
			}
			path_put(&nd->path);
		}
		nd->path = old_path;
		nd->last = last;
		nd->last_type = last_type;
	}
	return 1;
}