xmlCreateEntity(xmlDictPtr dict, const xmlChar *name, int type,
	        const xmlChar *ExternalID, const xmlChar *SystemID,
	        const xmlChar *content) {
    xmlEntityPtr ret;

    ret = (xmlEntityPtr) xmlMalloc(sizeof(xmlEntity));
    if (ret == NULL) {
        xmlEntitiesErrMemory("xmlCreateEntity: malloc failed");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlEntity));
    ret->type = XML_ENTITY_DECL;
    ret->checked = 0;

    /*
     * fill the structure.
     */
    ret->etype = (xmlEntityType) type;
    if (dict == NULL) {
	ret->name = xmlStrdup(name);
	if (ExternalID != NULL)
	    ret->ExternalID = xmlStrdup(ExternalID);
	if (SystemID != NULL)
	    ret->SystemID = xmlStrdup(SystemID);
    } else {
        ret->name = xmlDictLookup(dict, name, -1);
	if (ExternalID != NULL)
	    ret->ExternalID = xmlDictLookup(dict, ExternalID, -1);
	if (SystemID != NULL)
	    ret->SystemID = xmlDictLookup(dict, SystemID, -1);
    }
    if (content != NULL) {
        ret->length = xmlStrlen(content);
	if ((dict != NULL) && (ret->length < 5))
	    ret->content = (xmlChar *)
	                   xmlDictLookup(dict, content, ret->length);
	else
	    ret->content = xmlStrndup(content, ret->length);
     } else {
        ret->length = 0;
        ret->content = NULL;
    }
    ret->URI = NULL; /* to be computed by the layer knowing
			the defining entity */
    ret->orig = NULL;
    ret->owner = 0;

    return(ret);
}