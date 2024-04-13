xmlParseExternalEntityPrivate(xmlDocPtr doc, xmlParserCtxtPtr oldctxt,
	              xmlSAXHandlerPtr sax,
		      void *user_data, int depth, const xmlChar *URL,
		      const xmlChar *ID, xmlNodePtr *list) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc;
    xmlNodePtr newRoot;
    xmlSAXHandlerPtr oldsax = NULL;
    xmlParserErrors ret = XML_ERR_OK;
    xmlChar start[4];
    xmlCharEncoding enc;

    if (((depth > 40) &&
	((oldctxt == NULL) || (oldctxt->options & XML_PARSE_HUGE) == 0)) ||
	(depth > 1024)) {
	return(XML_ERR_ENTITY_LOOP);
    }

    if (list != NULL)
        *list = NULL;
    if ((URL == NULL) && (ID == NULL))
	return(XML_ERR_INTERNAL_ERROR);
    if (doc == NULL)
	return(XML_ERR_INTERNAL_ERROR);


    ctxt = xmlCreateEntityParserCtxtInternal(URL, ID, NULL, oldctxt);
    if (ctxt == NULL) return(XML_WAR_UNDECLARED_ENTITY);
    ctxt->userData = ctxt;
    if (oldctxt != NULL) {
	ctxt->_private = oldctxt->_private;
	ctxt->loadsubset = oldctxt->loadsubset;
	ctxt->validate = oldctxt->validate;
	ctxt->external = oldctxt->external;
	ctxt->record_info = oldctxt->record_info;
	ctxt->node_seq.maximum = oldctxt->node_seq.maximum;
	ctxt->node_seq.length = oldctxt->node_seq.length;
	ctxt->node_seq.buffer = oldctxt->node_seq.buffer;
    } else {
	/*
	 * Doing validity checking on chunk without context
	 * doesn't make sense
	 */
	ctxt->_private = NULL;
	ctxt->validate = 0;
	ctxt->external = 2;
	ctxt->loadsubset = 0;
    }
    if (sax != NULL) {
	oldsax = ctxt->sax;
        ctxt->sax = sax;
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }
    xmlDetectSAX2(ctxt);
    newDoc = xmlNewDoc(BAD_CAST "1.0");
    if (newDoc == NULL) {
	ctxt->node_seq.maximum = 0;
	ctxt->node_seq.length = 0;
	ctxt->node_seq.buffer = NULL;
	xmlFreeParserCtxt(ctxt);
	return(XML_ERR_INTERNAL_ERROR);
    }
    newDoc->properties = XML_DOC_INTERNAL;
    newDoc->intSubset = doc->intSubset;
    newDoc->extSubset = doc->extSubset;
    newDoc->dict = doc->dict;
    xmlDictReference(newDoc->dict);

    if (doc->URL != NULL) {
	newDoc->URL = xmlStrdup(doc->URL);
    }
    newRoot = xmlNewDocNode(newDoc, NULL, BAD_CAST "pseudoroot", NULL);
    if (newRoot == NULL) {
	if (sax != NULL)
	    ctxt->sax = oldsax;
	ctxt->node_seq.maximum = 0;
	ctxt->node_seq.length = 0;
	ctxt->node_seq.buffer = NULL;
	xmlFreeParserCtxt(ctxt);
	newDoc->intSubset = NULL;
	newDoc->extSubset = NULL;
        xmlFreeDoc(newDoc);
	return(XML_ERR_INTERNAL_ERROR);
    }
    xmlAddChild((xmlNodePtr) newDoc, newRoot);
    nodePush(ctxt, newDoc->children);
    ctxt->myDoc = doc;
    newRoot->doc = doc;

    /*
     * Get the 4 first bytes and decode the charset
     * if enc != XML_CHAR_ENCODING_NONE
     * plug some encoding conversion routines.
     */
    GROW;
    if ((ctxt->input->end - ctxt->input->cur) >= 4) {
	start[0] = RAW;
	start[1] = NXT(1);
	start[2] = NXT(2);
	start[3] = NXT(3);
	enc = xmlDetectCharEncoding(start, 4);
	if (enc != XML_CHAR_ENCODING_NONE) {
	    xmlSwitchEncoding(ctxt, enc);
	}
    }

    /*
     * Parse a possible text declaration first
     */
    if ((CMP5(CUR_PTR, '<', '?', 'x', 'm', 'l')) && (IS_BLANK_CH(NXT(5)))) {
	xmlParseTextDecl(ctxt);
    }

    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->depth = depth;

    xmlParseContent(ctxt);

    if ((RAW == '<') && (NXT(1) == '/')) {
	xmlFatalErr(ctxt, XML_ERR_NOT_WELL_BALANCED, NULL);
    } else if (RAW != 0) {
	xmlFatalErr(ctxt, XML_ERR_EXTRA_CONTENT, NULL);
    }
    if (ctxt->node != newDoc->children) {
	xmlFatalErr(ctxt, XML_ERR_NOT_WELL_BALANCED, NULL);
    }

    if (!ctxt->wellFormed) {
        if (ctxt->errNo == 0)
	    ret = XML_ERR_INTERNAL_ERROR;
	else
	    ret = (xmlParserErrors)ctxt->errNo;
    } else {
	if (list != NULL) {
	    xmlNodePtr cur;

	    /*
	     * Return the newly created nodeset after unlinking it from
	     * they pseudo parent.
	     */
	    cur = newDoc->children->children;
	    *list = cur;
	    while (cur != NULL) {
		cur->parent = NULL;
		cur = cur->next;
	    }
            newDoc->children->children = NULL;
	}
	ret = XML_ERR_OK;
    }

    /*
     * Record in the parent context the number of entities replacement
     * done when parsing that reference.
     */
    if (oldctxt != NULL)
        oldctxt->nbentities += ctxt->nbentities;

    /*
     * Also record the size of the entity parsed
     */
    if (ctxt->input != NULL && oldctxt != NULL) {
	oldctxt->sizeentities += ctxt->input->consumed;
	oldctxt->sizeentities += (ctxt->input->cur - ctxt->input->base);
    }
    /*
     * And record the last error if any
     */
    if ((oldctxt != NULL) && (ctxt->lastError.code != XML_ERR_OK))
        xmlCopyError(&ctxt->lastError, &oldctxt->lastError);

    if (sax != NULL)
	ctxt->sax = oldsax;
    if (oldctxt != NULL) {
        oldctxt->node_seq.maximum = ctxt->node_seq.maximum;
        oldctxt->node_seq.length = ctxt->node_seq.length;
        oldctxt->node_seq.buffer = ctxt->node_seq.buffer;
    }
    ctxt->node_seq.maximum = 0;
    ctxt->node_seq.length = 0;
    ctxt->node_seq.buffer = NULL;
    xmlFreeParserCtxt(ctxt);
    newDoc->intSubset = NULL;
    newDoc->extSubset = NULL;
    xmlFreeDoc(newDoc);

    return(ret);
}