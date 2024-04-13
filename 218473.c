xmlAttrPtr get_attribute_ex(xmlAttrPtr node, char *name, char *ns)
{
	while (node!=NULL) {
		if (attr_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}