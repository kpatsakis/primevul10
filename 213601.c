ff_layout_encode_ff_iostat(struct xdr_stream *xdr,
			    const nfs4_stateid *stateid,
			    const struct nfs42_layoutstat_devinfo *devinfo)
{
	ff_layout_encode_ff_iostat_head(xdr, stateid, devinfo);
	ff_layout_encode_ff_layoutupdate(xdr, devinfo,
			devinfo->ld_private.data);
}