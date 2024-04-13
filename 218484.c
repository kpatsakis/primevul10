xmlNodePtr get_node_ex(xmlNodePtr node, char *name, char *ns)
{
	while (node!=NULL) {
		if (node_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}