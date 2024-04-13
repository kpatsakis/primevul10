static int do_path_lookup(int dfd, const char *name,
				unsigned int flags, struct nameidata *nd)
{
	int retval = 0;
	int fput_needed;
	struct file *file;
	struct fs_struct *fs = current->fs;

	nd->last_type = LAST_ROOT; /* if there are only slashes... */
	nd->flags = flags;
	nd->depth = 0;

	if (*name=='/') {
		read_lock(&fs->lock);
		if (fs->altroot.dentry && !(nd->flags & LOOKUP_NOALT)) {
			nd->path = fs->altroot;
			path_get(&fs->altroot);
			read_unlock(&fs->lock);
			if (__emul_lookup_dentry(name,nd))
				goto out; /* found in altroot */
			read_lock(&fs->lock);
		}
		nd->path = fs->root;
		path_get(&fs->root);
		read_unlock(&fs->lock);
	} else if (dfd == AT_FDCWD) {
		read_lock(&fs->lock);
		nd->path = fs->pwd;
		path_get(&fs->pwd);
		read_unlock(&fs->lock);
	} else {
		struct dentry *dentry;

		file = fget_light(dfd, &fput_needed);
		retval = -EBADF;
		if (!file)
			goto out_fail;

		dentry = file->f_path.dentry;

		retval = -ENOTDIR;
		if (!S_ISDIR(dentry->d_inode->i_mode))
			goto fput_fail;

		retval = file_permission(file, MAY_EXEC);
		if (retval)
			goto fput_fail;

		nd->path = file->f_path;
		path_get(&file->f_path);

		fput_light(file, fput_needed);
	}

	retval = path_walk(name, nd);
out:
	if (unlikely(!retval && !audit_dummy_context() && nd->path.dentry &&
				nd->path.dentry->d_inode))
		audit_inode(name, nd->path.dentry);
out_fail:
	return retval;

fput_fail:
	fput_light(file, fput_needed);
	goto out_fail;
}