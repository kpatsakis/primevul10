struct file *shmem_file_setup(char *name, loff_t size, unsigned long flags)
{
	int error;
	struct file *file;
	struct inode *inode;
	struct dentry *dentry, *root;
	struct qstr this;

	if (IS_ERR(shm_mnt))
		return (void *)shm_mnt;

	if (size < 0 || size > SHMEM_MAX_BYTES)
		return ERR_PTR(-EINVAL);

	if (shmem_acct_size(flags, size))
		return ERR_PTR(-ENOMEM);

	error = -ENOMEM;
	this.name = name;
	this.len = strlen(name);
	this.hash = 0; /* will go */
	root = shm_mnt->mnt_root;
	dentry = d_alloc(root, &this);
	if (!dentry)
		goto put_memory;

	error = -ENFILE;
	file = get_empty_filp();
	if (!file)
		goto put_dentry;

	error = -ENOSPC;
	inode = shmem_get_inode(root->d_sb, S_IFREG | S_IRWXUGO, 0);
	if (!inode)
		goto close_file;

	SHMEM_I(inode)->flags = flags & VM_ACCOUNT;
	d_instantiate(dentry, inode);
	inode->i_size = size;
	inode->i_nlink = 0;	/* It is unlinked */
	init_file(file, shm_mnt, dentry, FMODE_WRITE | FMODE_READ,
			&shmem_file_operations);
	return file;

close_file:
	put_filp(file);
put_dentry:
	dput(dentry);
put_memory:
	shmem_unacct_size(flags, size);
	return ERR_PTR(error);
}