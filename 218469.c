static void cleanup_xml_node(xmlNodePtr node)
{
	xmlNodePtr trav;
	xmlNodePtr del = NULL;

	trav = node->children;
	while (trav != NULL) {
		if (del != NULL) {
			xmlUnlinkNode(del);
			xmlFreeNode(del);
			del = NULL;
		}
		if (trav->type == XML_TEXT_NODE) {
			if (is_blank(trav->content)) {
				del = trav;
			}
		} else if ((trav->type != XML_ELEMENT_NODE) &&
		           (trav->type != XML_CDATA_SECTION_NODE)) {
			del = trav;
		} else if (trav->children != NULL) {
			cleanup_xml_node(trav);
		}
		trav = trav->next;
	}
	if (del != NULL) {
		xmlUnlinkNode(del);
		xmlFreeNode(del);
	}
}