decode_name(struct xdr_stream *xdr, u32 *id)
{
	__be32 *p;
	int len;

	/* opaque_length(4)*/
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -ENOBUFS;
	len = be32_to_cpup(p++);
	if (len < 0)
		return -EINVAL;

	dprintk("%s: len %u\n", __func__, len);

	/* opaque body */
	p = xdr_inline_decode(xdr, len);
	if (unlikely(!p))
		return -ENOBUFS;

	if (!nfs_map_string_to_numeric((char *)p, len, id))
		return -EINVAL;

	return 0;
}