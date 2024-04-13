ff_layout_free_layoutreturn(struct nfs4_xdr_opaque_data *args)
{
	struct nfs4_flexfile_layoutreturn_args *ff_args;

	if (!args->data)
		return;
	ff_args = args->data;
	args->data = NULL;

	ff_layout_free_ds_ioerr(&ff_args->errors);
	ff_layout_free_iostats_array(ff_args->devinfo, ff_args->num_dev);

	put_page(ff_args->pages[0]);
	kfree(ff_args);
}