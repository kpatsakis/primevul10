int addServiceTable(
	IXML_Node *node, service_table *in, const char *DefaultURLBase)
{
	IXML_Node *root = NULL;
	IXML_Node *URLBase = NULL;
	service_info *tempEnd = NULL;

	if (in->URLBase) {
		free(in->URLBase);
		in->URLBase = NULL;
	}
	if (getSubElement("root", node, &root)) {
		if (getSubElement("URLBase", root, &URLBase)) {
			in->URLBase = getElementValue(URLBase);
		} else {
			if (DefaultURLBase) {
				in->URLBase =
					ixmlCloneDOMString(DefaultURLBase);
			} else {
				in->URLBase = ixmlCloneDOMString("");
			}
		}
		if ((in->endServiceList->next = getAllServiceList(
			     root, in->URLBase, &tempEnd))) {
			in->endServiceList = tempEnd;
			return 1;
		}
	}

	return 0;
}