ff_layout_encode_ff_iostat_head(struct xdr_stream *xdr,
			    const nfs4_stateid *stateid,
			    const struct nfs42_layoutstat_devinfo *devinfo)
{
	__be32 *p;

	p = xdr_reserve_space(xdr, 8 + 8);
	p = xdr_encode_hyper(p, devinfo->offset);
	p = xdr_encode_hyper(p, devinfo->length);
	encode_opaque_fixed(xdr, stateid->data, NFS4_STATEID_SIZE);
	p = xdr_reserve_space(xdr, 4*8);
	p = xdr_encode_hyper(p, devinfo->read_count);
	p = xdr_encode_hyper(p, devinfo->read_bytes);
	p = xdr_encode_hyper(p, devinfo->write_count);
	p = xdr_encode_hyper(p, devinfo->write_bytes);
	encode_opaque_fixed(xdr, devinfo->dev_id.data, NFS4_DEVICEID4_SIZE);
}