void set_fs_altroot(void)
{
	char *emul = __emul_prefix();
	struct nameidata nd;
	struct path path = {}, old_path;
	int err;
	struct fs_struct *fs = current->fs;

	if (!emul)
		goto set_it;
	err = path_lookup(emul, LOOKUP_FOLLOW|LOOKUP_DIRECTORY|LOOKUP_NOALT, &nd);
	if (!err)
		path = nd.path;
set_it:
	write_lock(&fs->lock);
	old_path = fs->altroot;
	fs->altroot = path;
	write_unlock(&fs->lock);
	if (old_path.dentry)
		path_put(&old_path);
}