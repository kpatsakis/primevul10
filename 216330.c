DOMString getElementValue(IXML_Node *node)
{
	IXML_Node *child = (IXML_Node *)ixmlNode_getFirstChild(node);
	const DOMString temp = NULL;

	if (child && ixmlNode_getNodeType(child) == eTEXT_NODE) {
		temp = ixmlNode_getNodeValue(child);

		return ixmlCloneDOMString(temp);
	} else {
		return NULL;
	}
}