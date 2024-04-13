ff_layout_encode_layoutstats(struct xdr_stream *xdr, const void *args,
			     const struct nfs4_xdr_opaque_data *opaque)
{
	struct nfs42_layoutstat_devinfo *devinfo = container_of(opaque,
			struct nfs42_layoutstat_devinfo, ld_private);
	__be32 *start;

	/* layoutupdate length */
	start = xdr_reserve_space(xdr, 4);
	ff_layout_encode_ff_layoutupdate(xdr, devinfo, opaque->data);

	*start = cpu_to_be32((xdr->p - start - 1) * 4);
}