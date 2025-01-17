void umount_tree(struct vfsmount *mnt, int propagate, struct list_head *kill)
{
	struct vfsmount *p;

	for (p = mnt; p; p = next_mnt(p, mnt))
		list_move(&p->mnt_hash, kill);

	if (propagate)
		propagate_umount(kill);

	list_for_each_entry(p, kill, mnt_hash) {
		list_del_init(&p->mnt_expire);
		list_del_init(&p->mnt_list);
		__touch_mnt_namespace(p->mnt_ns);
		p->mnt_ns = NULL;
		list_del_init(&p->mnt_child);
		if (p->mnt_parent != p)
			p->mnt_mountpoint->d_mounted--;
		change_mnt_propagation(p, MS_PRIVATE);
	}
}