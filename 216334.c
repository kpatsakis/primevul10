int getServiceTable(
	IXML_Node *node, service_table *out, const char *DefaultURLBase)
{
	IXML_Node *root = NULL;
	IXML_Node *URLBase = NULL;

	if (getSubElement("root", node, &root)) {
		if (getSubElement("URLBase", root, &URLBase)) {
			out->URLBase = getElementValue(URLBase);
		} else {
			if (DefaultURLBase) {
				out->URLBase =
					ixmlCloneDOMString(DefaultURLBase);
			} else {
				out->URLBase = ixmlCloneDOMString("");
			}
		}
		out->serviceList = getAllServiceList(
			root, out->URLBase, &out->endServiceList);
		if (out->serviceList) {
			return 1;
		}
	}

	return 0;
}