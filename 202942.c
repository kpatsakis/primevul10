clusterip_del_node(struct clusterip_config *c, u_int16_t nodenum)
{
	if (nodenum == 0 ||
	    nodenum > c->num_total_nodes)
		return true;

	if (test_and_clear_bit(nodenum - 1, &c->local_nodes))
		return false;

	return true;
}