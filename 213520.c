ff_layout_free_layout_hdr(struct pnfs_layout_hdr *lo)
{
	struct nfs4_flexfile_layout *ffl = FF_LAYOUT_FROM_HDR(lo);
	struct nfs4_ff_layout_ds_err *err, *n;

	list_for_each_entry_safe(err, n, &ffl->error_list, list) {
		list_del(&err->list);
		kfree(err);
	}
	kfree_rcu(ffl, generic_hdr.plh_rcu);
}