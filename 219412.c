xmlNewEntity(xmlDocPtr doc, const xmlChar *name, int type,
	     const xmlChar *ExternalID, const xmlChar *SystemID,
	     const xmlChar *content) {
    xmlEntityPtr ret;
    xmlDictPtr dict;

    if ((doc != NULL) && (doc->intSubset != NULL)) {
	return(xmlAddDocEntity(doc, name, type, ExternalID, SystemID, content));
    }
    if (doc != NULL)
        dict = doc->dict;
    else
        dict = NULL;
    ret = xmlCreateEntity(dict, name, type, ExternalID, SystemID, content);
    if (ret == NULL)
        return(NULL);
    ret->doc = doc;
    return(ret);
}