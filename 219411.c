xmlAddDocEntity(xmlDocPtr doc, const xmlChar *name, int type,
	        const xmlChar *ExternalID, const xmlChar *SystemID,
	        const xmlChar *content) {
    xmlEntityPtr ret;
    xmlDtdPtr dtd;

    if (doc == NULL) {
	xmlEntitiesErr(XML_DTD_NO_DOC,
	        "xmlAddDocEntity: document is NULL");
	return(NULL);
    }
    if (doc->intSubset == NULL) {
	xmlEntitiesErr(XML_DTD_NO_DTD,
	        "xmlAddDocEntity: document without internal subset");
	return(NULL);
    }
    dtd = doc->intSubset;
    ret = xmlAddEntity(dtd, name, type, ExternalID, SystemID, content);
    if (ret == NULL) return(NULL);

    /*
     * Link it to the DTD
     */
    ret->parent = dtd;
    ret->doc = dtd->doc;
    if (dtd->last == NULL) {
	dtd->children = dtd->last = (xmlNodePtr) ret;
    } else {
	dtd->last->next = (xmlNodePtr) ret;
	ret->prev = dtd->last;
	dtd->last = (xmlNodePtr) ret;
    }
    return(ret);
}