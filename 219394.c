xmlCopyEntity(void *payload, const xmlChar *name ATTRIBUTE_UNUSED) {
    xmlEntityPtr ent = (xmlEntityPtr) payload;
    xmlEntityPtr cur;

    cur = (xmlEntityPtr) xmlMalloc(sizeof(xmlEntity));
    if (cur == NULL) {
        xmlEntitiesErrMemory("xmlCopyEntity:: malloc failed");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlEntity));
    cur->type = XML_ENTITY_DECL;

    cur->etype = ent->etype;
    if (ent->name != NULL)
	cur->name = xmlStrdup(ent->name);
    if (ent->ExternalID != NULL)
	cur->ExternalID = xmlStrdup(ent->ExternalID);
    if (ent->SystemID != NULL)
	cur->SystemID = xmlStrdup(ent->SystemID);
    if (ent->content != NULL)
	cur->content = xmlStrdup(ent->content);
    if (ent->orig != NULL)
	cur->orig = xmlStrdup(ent->orig);
    if (ent->URI != NULL)
	cur->URI = xmlStrdup(ent->URI);
    return(cur);
}