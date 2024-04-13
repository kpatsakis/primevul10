ff_layout_prepare_layoutreturn(struct nfs4_layoutreturn_args *args)
{
	struct nfs4_flexfile_layoutreturn_args *ff_args;
	struct nfs4_flexfile_layout *ff_layout = FF_LAYOUT_FROM_HDR(args->layout);

	ff_args = kmalloc(sizeof(*ff_args), GFP_KERNEL);
	if (!ff_args)
		goto out_nomem;
	ff_args->pages[0] = alloc_page(GFP_KERNEL);
	if (!ff_args->pages[0])
		goto out_nomem_free;

	INIT_LIST_HEAD(&ff_args->errors);
	ff_args->num_errors = ff_layout_fetch_ds_ioerr(args->layout,
			&args->range, &ff_args->errors,
			FF_LAYOUTRETURN_MAXERR);

	spin_lock(&args->inode->i_lock);
	ff_args->num_dev = ff_layout_mirror_prepare_stats(&ff_layout->generic_hdr,
			&ff_args->devinfo[0], ARRAY_SIZE(ff_args->devinfo));
	spin_unlock(&args->inode->i_lock);

	args->ld_private->ops = &layoutreturn_ops;
	args->ld_private->data = ff_args;
	return 0;
out_nomem_free:
	kfree(ff_args);
out_nomem:
	return -ENOMEM;
}