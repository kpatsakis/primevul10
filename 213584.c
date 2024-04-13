ff_layout_free_deviceid_node(struct nfs4_deviceid_node *d)
{
	nfs4_ff_layout_free_deviceid(container_of(d, struct nfs4_ff_layout_ds,
						  id_node));
}