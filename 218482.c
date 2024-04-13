xmlNsPtr attr_find_ns(xmlAttrPtr node)
{
	if (node->ns) {
		return node->ns;
	} else if (node->parent->ns) {
		return node->parent->ns;
	} else {
		return xmlSearchNs(node->doc, node->parent, NULL);
	}
}