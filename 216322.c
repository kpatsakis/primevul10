int getSubElement(const char *element_name, IXML_Node *node, IXML_Node **out)
{
	const DOMString NodeName = NULL;
	int found = 0;
	IXML_Node *child = (IXML_Node *)ixmlNode_getFirstChild(node);

	(*out) = NULL;
	while (child && !found) {
		switch (ixmlNode_getNodeType(child)) {
		case eELEMENT_NODE:
			NodeName = ixmlNode_getNodeName(child);
			if (!strcmp(NodeName, element_name)) {
				(*out) = child;
				found = 1;
				return found;
			}
			break;
		default:
			break;
		}
		child = (IXML_Node *)ixmlNode_getNextSibling(child);
	}

	return found;
}