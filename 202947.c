clusterip_add_node(struct clusterip_config *c, u_int16_t nodenum)
{

	if (nodenum == 0 ||
	    nodenum > c->num_total_nodes)
		return 1;

	/* check if we already have this number in our bitfield */
	if (test_and_set_bit(nodenum - 1, &c->local_nodes))
		return 1;

	return 0;
}