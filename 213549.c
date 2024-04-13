ff_layout_send_layouterror(struct pnfs_layout_segment *lseg)
{
	struct pnfs_layout_hdr *lo = lseg->pls_layout;
	struct nfs42_layout_error *errors;
	LIST_HEAD(head);

	if (!nfs_server_capable(lo->plh_inode, NFS_CAP_LAYOUTERROR))
		return;
	ff_layout_fetch_ds_ioerr(lo, &lseg->pls_range, &head, -1);
	if (list_empty(&head))
		return;

	errors = kmalloc_array(NFS42_LAYOUTERROR_MAX,
			sizeof(*errors), GFP_NOFS);
	if (errors != NULL) {
		const struct nfs4_ff_layout_ds_err *pos;
		size_t n = 0;

		list_for_each_entry(pos, &head, list) {
			errors[n].offset = pos->offset;
			errors[n].length = pos->length;
			nfs4_stateid_copy(&errors[n].stateid, &pos->stateid);
			errors[n].errors[0].dev_id = pos->deviceid;
			errors[n].errors[0].status = pos->status;
			errors[n].errors[0].opnum = pos->opnum;
			n++;
			if (!list_is_last(&pos->list, &head) &&
			    n < NFS42_LAYOUTERROR_MAX)
				continue;
			if (nfs42_proc_layouterror(lseg, errors, n) < 0)
				break;
			n = 0;
		}
		kfree(errors);
	}
	ff_layout_free_ds_ioerr(&head);
}