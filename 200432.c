static __always_inline void follow_dotdot(struct nameidata *nd)
{
	struct fs_struct *fs = current->fs;

	while(1) {
		struct vfsmount *parent;
		struct dentry *old = nd->path.dentry;

                read_lock(&fs->lock);
		if (nd->path.dentry == fs->root.dentry &&
		    nd->path.mnt == fs->root.mnt) {
                        read_unlock(&fs->lock);
			break;
		}
                read_unlock(&fs->lock);
		spin_lock(&dcache_lock);
		if (nd->path.dentry != nd->path.mnt->mnt_root) {
			nd->path.dentry = dget(nd->path.dentry->d_parent);
			spin_unlock(&dcache_lock);
			dput(old);
			break;
		}
		spin_unlock(&dcache_lock);
		spin_lock(&vfsmount_lock);
		parent = nd->path.mnt->mnt_parent;
		if (parent == nd->path.mnt) {
			spin_unlock(&vfsmount_lock);
			break;
		}
		mntget(parent);
		nd->path.dentry = dget(nd->path.mnt->mnt_mountpoint);
		spin_unlock(&vfsmount_lock);
		dput(old);
		mntput(nd->path.mnt);
		nd->path.mnt = parent;
	}
	follow_mount(&nd->path.mnt, &nd->path.dentry);
}