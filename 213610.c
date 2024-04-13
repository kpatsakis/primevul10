ff_layout_prepare_layoutstats(struct nfs42_layoutstat_args *args)
{
	struct nfs4_flexfile_layout *ff_layout;
	const int dev_count = PNFS_LAYOUTSTATS_MAXDEV;

	/* For now, send at most PNFS_LAYOUTSTATS_MAXDEV statistics */
	args->devinfo = kmalloc_array(dev_count, sizeof(*args->devinfo), GFP_NOIO);
	if (!args->devinfo)
		return -ENOMEM;

	spin_lock(&args->inode->i_lock);
	ff_layout = FF_LAYOUT_FROM_HDR(NFS_I(args->inode)->layout);
	args->num_dev = ff_layout_mirror_prepare_stats(&ff_layout->generic_hdr,
			&args->devinfo[0], dev_count);
	spin_unlock(&args->inode->i_lock);
	if (!args->num_dev) {
		kfree(args->devinfo);
		args->devinfo = NULL;
		return -ENOENT;
	}

	return 0;
}