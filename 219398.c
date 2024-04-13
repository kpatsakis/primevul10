xmlFreeEntity(xmlEntityPtr entity)
{
    xmlDictPtr dict = NULL;

    if (entity == NULL)
        return;

    if (entity->doc != NULL)
        dict = entity->doc->dict;


    if ((entity->children) && (entity->owner == 1) &&
        (entity == (xmlEntityPtr) entity->children->parent))
        xmlFreeNodeList(entity->children);
    if (dict != NULL) {
        if ((entity->name != NULL) && (!xmlDictOwns(dict, entity->name)))
            xmlFree((char *) entity->name);
        if ((entity->ExternalID != NULL) &&
	    (!xmlDictOwns(dict, entity->ExternalID)))
            xmlFree((char *) entity->ExternalID);
        if ((entity->SystemID != NULL) &&
	    (!xmlDictOwns(dict, entity->SystemID)))
            xmlFree((char *) entity->SystemID);
        if ((entity->URI != NULL) && (!xmlDictOwns(dict, entity->URI)))
            xmlFree((char *) entity->URI);
        if ((entity->content != NULL)
            && (!xmlDictOwns(dict, entity->content)))
            xmlFree((char *) entity->content);
        if ((entity->orig != NULL) && (!xmlDictOwns(dict, entity->orig)))
            xmlFree((char *) entity->orig);
    } else {
        if (entity->name != NULL)
            xmlFree((char *) entity->name);
        if (entity->ExternalID != NULL)
            xmlFree((char *) entity->ExternalID);
        if (entity->SystemID != NULL)
            xmlFree((char *) entity->SystemID);
        if (entity->URI != NULL)
            xmlFree((char *) entity->URI);
        if (entity->content != NULL)
            xmlFree((char *) entity->content);
        if (entity->orig != NULL)
            xmlFree((char *) entity->orig);
    }
    xmlFree(entity);
}