xmlParseBalancedChunkMemoryInternal(xmlParserCtxtPtr oldctxt,
	const xmlChar *string, void *user_data, xmlNodePtr *lst) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc = NULL;
    xmlNodePtr newRoot;
    xmlSAXHandlerPtr oldsax = NULL;
    xmlNodePtr content = NULL;
    xmlNodePtr last = NULL;
    int size;
    xmlParserErrors ret = XML_ERR_OK;
#ifdef SAX2
    int i;
#endif

    if (((oldctxt->depth > 40) && ((oldctxt->options & XML_PARSE_HUGE) == 0)) ||
        (oldctxt->depth >  1024)) {
	return(XML_ERR_ENTITY_LOOP);
    }


    if (lst != NULL)
        *lst = NULL;
    if (string == NULL)
        return(XML_ERR_INTERNAL_ERROR);

    size = xmlStrlen(string);

    ctxt = xmlCreateMemoryParserCtxt((char *) string, size);
    if (ctxt == NULL) return(XML_WAR_UNDECLARED_ENTITY);
    if (user_data != NULL)
	ctxt->userData = user_data;
    else
	ctxt->userData = ctxt;
    if (ctxt->dict != NULL) xmlDictFree(ctxt->dict);
    ctxt->dict = oldctxt->dict;
    ctxt->input_id = oldctxt->input_id + 1;
    ctxt->str_xml = xmlDictLookup(ctxt->dict, BAD_CAST "xml", 3);
    ctxt->str_xmlns = xmlDictLookup(ctxt->dict, BAD_CAST "xmlns", 5);
    ctxt->str_xml_ns = xmlDictLookup(ctxt->dict, XML_XML_NAMESPACE, 36);

#ifdef SAX2
    /* propagate namespaces down the entity */
    for (i = 0;i < oldctxt->nsNr;i += 2) {
        nsPush(ctxt, oldctxt->nsTab[i], oldctxt->nsTab[i+1]);
    }
#endif

    oldsax = ctxt->sax;
    ctxt->sax = oldctxt->sax;
    xmlDetectSAX2(ctxt);
    ctxt->replaceEntities = oldctxt->replaceEntities;
    ctxt->options = oldctxt->options;

    ctxt->_private = oldctxt->_private;
    if (oldctxt->myDoc == NULL) {
	newDoc = xmlNewDoc(BAD_CAST "1.0");
	if (newDoc == NULL) {
	    ctxt->sax = oldsax;
	    ctxt->dict = NULL;
	    xmlFreeParserCtxt(ctxt);
	    return(XML_ERR_INTERNAL_ERROR);
	}
	newDoc->properties = XML_DOC_INTERNAL;
	newDoc->dict = ctxt->dict;
	xmlDictReference(newDoc->dict);
	ctxt->myDoc = newDoc;
    } else {
	ctxt->myDoc = oldctxt->myDoc;
        content = ctxt->myDoc->children;
	last = ctxt->myDoc->last;
    }
    newRoot = xmlNewDocNode(ctxt->myDoc, NULL, BAD_CAST "pseudoroot", NULL);
    if (newRoot == NULL) {
	ctxt->sax = oldsax;
	ctxt->dict = NULL;
	xmlFreeParserCtxt(ctxt);
	if (newDoc != NULL) {
	    xmlFreeDoc(newDoc);
	}
	return(XML_ERR_INTERNAL_ERROR);
    }
    ctxt->myDoc->children = NULL;
    ctxt->myDoc->last = NULL;
    xmlAddChild((xmlNodePtr) ctxt->myDoc, newRoot);
    nodePush(ctxt, ctxt->myDoc->children);
    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->depth = oldctxt->depth + 1;

    ctxt->validate = 0;
    ctxt->loadsubset = oldctxt->loadsubset;
    if ((oldctxt->validate) || (oldctxt->replaceEntities != 0)) {
	/*
	 * ID/IDREF registration will be done in xmlValidateElement below
	 */
	ctxt->loadsubset |= XML_SKIP_IDS;
    }
    ctxt->dictNames = oldctxt->dictNames;
    ctxt->attsDefault = oldctxt->attsDefault;
    ctxt->attsSpecial = oldctxt->attsSpecial;

    xmlParseContent(ctxt);
    if ((RAW == '<') && (NXT(1) == '/')) {
	xmlFatalErr(ctxt, XML_ERR_NOT_WELL_BALANCED, NULL);
    } else if (RAW != 0) {
	xmlFatalErr(ctxt, XML_ERR_EXTRA_CONTENT, NULL);
    }
    if (ctxt->node != ctxt->myDoc->children) {
	xmlFatalErr(ctxt, XML_ERR_NOT_WELL_BALANCED, NULL);
    }

    if (!ctxt->wellFormed) {
        if (ctxt->errNo == 0)
	    ret = XML_ERR_INTERNAL_ERROR;
	else
	    ret = (xmlParserErrors)ctxt->errNo;
    } else {
      ret = XML_ERR_OK;
    }

    if ((lst != NULL) && (ret == XML_ERR_OK)) {
	xmlNodePtr cur;

	/*
	 * Return the newly created nodeset after unlinking it from
	 * they pseudo parent.
	 */
	cur = ctxt->myDoc->children->children;
	*lst = cur;
	while (cur != NULL) {
#ifdef LIBXML_VALID_ENABLED
	    if ((oldctxt->validate) && (oldctxt->wellFormed) &&
		(oldctxt->myDoc) && (oldctxt->myDoc->intSubset) &&
		(cur->type == XML_ELEMENT_NODE)) {
		oldctxt->valid &= xmlValidateElement(&oldctxt->vctxt,
			oldctxt->myDoc, cur);
	    }
#endif /* LIBXML_VALID_ENABLED */
	    cur->parent = NULL;
	    cur = cur->next;
	}
	ctxt->myDoc->children->children = NULL;
    }
    if (ctxt->myDoc != NULL) {
	xmlFreeNode(ctxt->myDoc->children);
        ctxt->myDoc->children = content;
        ctxt->myDoc->last = last;
    }

    /*
     * Record in the parent context the number of entities replacement
     * done when parsing that reference.
     */
    if (oldctxt != NULL)
        oldctxt->nbentities += ctxt->nbentities;

    /*
     * Also record the last error if any
     */
    if (ctxt->lastError.code != XML_ERR_OK)
        xmlCopyError(&ctxt->lastError, &oldctxt->lastError);

    ctxt->sax = oldsax;
    ctxt->dict = NULL;
    ctxt->attsDefault = NULL;
    ctxt->attsSpecial = NULL;
    xmlFreeParserCtxt(ctxt);
    if (newDoc != NULL) {
	xmlFreeDoc(newDoc);
    }

    return(ret);
}