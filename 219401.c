xmlAddEntity(xmlDtdPtr dtd, const xmlChar *name, int type,
	  const xmlChar *ExternalID, const xmlChar *SystemID,
	  const xmlChar *content) {
    xmlDictPtr dict = NULL;
    xmlEntitiesTablePtr table = NULL;
    xmlEntityPtr ret, predef;

    if (name == NULL)
	return(NULL);
    if (dtd == NULL)
	return(NULL);
    if (dtd->doc != NULL)
        dict = dtd->doc->dict;

    switch (type) {
        case XML_INTERNAL_GENERAL_ENTITY:
        case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
        case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
            predef = xmlGetPredefinedEntity(name);
            if (predef != NULL) {
                int valid = 0;

                /* 4.6 Predefined Entities */
                if ((type == XML_INTERNAL_GENERAL_ENTITY) &&
                    (content != NULL)) {
                    int c = predef->content[0];

                    if (((content[0] == c) && (content[1] == 0)) &&
                        ((c == '>') || (c == '\'') || (c == '"'))) {
                        valid = 1;
                    } else if ((content[0] == '&') && (content[1] == '#')) {
                        if (content[2] == 'x') {
                            xmlChar *hex = BAD_CAST "0123456789ABCDEF";
                            xmlChar ref[] = "00;";

                            ref[0] = hex[c / 16 % 16];
                            ref[1] = hex[c % 16];
                            if (xmlStrcasecmp(&content[3], ref) == 0)
                                valid = 1;
                        } else {
                            xmlChar ref[] = "00;";

                            ref[0] = '0' + c / 10 % 10;
                            ref[1] = '0' + c % 10;
                            if (xmlStrEqual(&content[2], ref))
                                valid = 1;
                        }
                    }
                }
                if (!valid) {
                    xmlEntitiesErr(XML_ERR_ENTITY_PROCESSING,
                            "xmlAddEntity: invalid redeclaration of predefined"
                            " entity");
                    return(NULL);
                }
            }
	    if (dtd->entities == NULL)
		dtd->entities = xmlHashCreateDict(0, dict);
	    table = dtd->entities;
	    break;
        case XML_INTERNAL_PARAMETER_ENTITY:
        case XML_EXTERNAL_PARAMETER_ENTITY:
	    if (dtd->pentities == NULL)
		dtd->pentities = xmlHashCreateDict(0, dict);
	    table = dtd->pentities;
	    break;
        case XML_INTERNAL_PREDEFINED_ENTITY:
	    return(NULL);
    }
    if (table == NULL)
	return(NULL);
    ret = xmlCreateEntity(dict, name, type, ExternalID, SystemID, content);
    if (ret == NULL)
        return(NULL);
    ret->doc = dtd->doc;

    if (xmlHashAddEntry(table, name, ret)) {
	/*
	 * entity was already defined at another level.
	 */
        xmlFreeEntity(ret);
	return(NULL);
    }
    return(ret);
}