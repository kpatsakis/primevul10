ff_layout_mark_ds_unreachable(struct pnfs_layout_segment *lseg, u32 idx)
{
	struct nfs4_deviceid_node *devid = FF_LAYOUT_DEVID_NODE(lseg, idx);

	if (devid)
		nfs4_mark_deviceid_unavailable(devid);
}