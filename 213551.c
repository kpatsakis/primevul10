ff_layout_mirror_prepare_stats(struct pnfs_layout_hdr *lo,
			       struct nfs42_layoutstat_devinfo *devinfo,
			       int dev_limit)
{
	struct nfs4_flexfile_layout *ff_layout = FF_LAYOUT_FROM_HDR(lo);
	struct nfs4_ff_layout_mirror *mirror;
	struct nfs4_deviceid_node *dev;
	int i = 0;

	list_for_each_entry(mirror, &ff_layout->mirrors, mirrors) {
		if (i >= dev_limit)
			break;
		if (IS_ERR_OR_NULL(mirror->mirror_ds))
			continue;
		if (!test_and_clear_bit(NFS4_FF_MIRROR_STAT_AVAIL, &mirror->flags))
			continue;
		/* mirror refcount put in cleanup_layoutstats */
		if (!refcount_inc_not_zero(&mirror->ref))
			continue;
		dev = &mirror->mirror_ds->id_node; 
		memcpy(&devinfo->dev_id, &dev->deviceid, NFS4_DEVICEID4_SIZE);
		devinfo->offset = 0;
		devinfo->length = NFS4_MAX_UINT64;
		spin_lock(&mirror->lock);
		devinfo->read_count = mirror->read_stat.io_stat.ops_completed;
		devinfo->read_bytes = mirror->read_stat.io_stat.bytes_completed;
		devinfo->write_count = mirror->write_stat.io_stat.ops_completed;
		devinfo->write_bytes = mirror->write_stat.io_stat.bytes_completed;
		spin_unlock(&mirror->lock);
		devinfo->layout_type = LAYOUT_FLEX_FILES;
		devinfo->ld_private.ops = &layoutstat_ops;
		devinfo->ld_private.data = mirror;

		devinfo++;
		i++;
	}
	return i;
}