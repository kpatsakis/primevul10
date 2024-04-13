ff_layout_free_layoutstats(struct nfs4_xdr_opaque_data *opaque)
{
	struct nfs4_ff_layout_mirror *mirror = opaque->data;

	ff_layout_put_mirror(mirror);
}