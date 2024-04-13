ff_layout_encode_layoutreturn(struct xdr_stream *xdr,
		const void *voidargs,
		const struct nfs4_xdr_opaque_data *ff_opaque)
{
	const struct nfs4_layoutreturn_args *args = voidargs;
	struct nfs4_flexfile_layoutreturn_args *ff_args = ff_opaque->data;
	struct xdr_buf tmp_buf = {
		.head = {
			[0] = {
				.iov_base = page_address(ff_args->pages[0]),
			},
		},
		.buflen = PAGE_SIZE,
	};
	struct xdr_stream tmp_xdr;
	__be32 *start;

	dprintk("%s: Begin\n", __func__);

	xdr_init_encode(&tmp_xdr, &tmp_buf, NULL, NULL);

	ff_layout_encode_ioerr(&tmp_xdr, args, ff_args);
	ff_layout_encode_iostats_array(&tmp_xdr, args, ff_args);

	start = xdr_reserve_space(xdr, 4);
	*start = cpu_to_be32(tmp_buf.len);
	xdr_write_pages(xdr, ff_args->pages, 0, tmp_buf.len);

	dprintk("%s: Return\n", __func__);
}