xmlNodePtr get_node_recurisve_ex(xmlNodePtr node, char *name, char *ns)
{
	while (node != NULL) {
		if (node_is_equal_ex(node, name, ns)) {
			return node;
		} else if (node->children != NULL) {
			xmlNodePtr tmp = get_node_recurisve_ex(node->children, name, ns);
			if (tmp) {
				return tmp;
			}
		}
		node = node->next;
	}
	return NULL;
}