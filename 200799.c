int hfs_cat_delete(u32 cnid, struct inode *dir, struct qstr *str)
{
	struct super_block *sb;
	struct hfs_find_data fd;
	struct list_head *pos;
	int res, type;

	dprint(DBG_CAT_MOD, "delete_cat: %s,%u\n", str ? str->name : NULL, cnid);
	sb = dir->i_sb;
	hfs_find_init(HFS_SB(sb)->cat_tree, &fd);

	hfs_cat_build_key(sb, fd.search_key, dir->i_ino, str);
	res = hfs_brec_find(&fd);
	if (res)
		goto out;

	type = hfs_bnode_read_u8(fd.bnode, fd.entryoffset);
	if (type == HFS_CDR_FIL) {
		struct hfs_cat_file file;
		hfs_bnode_read(fd.bnode, &file, fd.entryoffset, sizeof(file));
		if (be32_to_cpu(file.FlNum) == cnid) {
#if 0
			hfs_free_fork(sb, &file, HFS_FK_DATA);
#endif
			hfs_free_fork(sb, &file, HFS_FK_RSRC);
		}
	}

	list_for_each(pos, &HFS_I(dir)->open_dir_list) {
		struct hfs_readdir_data *rd =
			list_entry(pos, struct hfs_readdir_data, list);
		if (fd.tree->keycmp(fd.search_key, (void *)&rd->key) < 0)
			rd->file->f_pos--;
	}

	res = hfs_brec_remove(&fd);
	if (res)
		goto out;

	hfs_cat_build_key(sb, fd.search_key, cnid, NULL);
	res = hfs_brec_find(&fd);
	if (!res) {
		res = hfs_brec_remove(&fd);
		if (res)
			goto out;
	}

	dir->i_size--;
	dir->i_mtime = dir->i_ctime = CURRENT_TIME_SEC;
	mark_inode_dirty(dir);
	res = 0;
out:
	hfs_find_exit(&fd);

	return res;
}