xmlParseQName(xmlParserCtxtPtr ctxt, const xmlChar **prefix) {
    const xmlChar *l, *p;

    GROW;

    l = xmlParseNCName(ctxt);
    if (l == NULL) {
        if (CUR == ':') {
	    l = xmlParseName(ctxt);
	    if (l != NULL) {
	        xmlNsErr(ctxt, XML_NS_ERR_QNAME,
		         "Failed to parse QName '%s'\n", l, NULL, NULL);
		*prefix = NULL;
		return(l);
	    }
	}
        return(NULL);
    }
    if (CUR == ':') {
        NEXT;
	p = l;
	l = xmlParseNCName(ctxt);
	if (l == NULL) {
	    xmlChar *tmp;

            if (ctxt->instate == XML_PARSER_EOF)
                return(NULL);
            xmlNsErr(ctxt, XML_NS_ERR_QNAME,
	             "Failed to parse QName '%s:'\n", p, NULL, NULL);
	    l = xmlParseNmtoken(ctxt);
	    if (l == NULL) {
                if (ctxt->instate == XML_PARSER_EOF)
                    return(NULL);
		tmp = xmlBuildQName(BAD_CAST "", p, NULL, 0);
            } else {
		tmp = xmlBuildQName(l, p, NULL, 0);
		xmlFree((char *)l);
	    }
	    p = xmlDictLookup(ctxt->dict, tmp, -1);
	    if (tmp != NULL) xmlFree(tmp);
	    *prefix = NULL;
	    return(p);
	}
	if (CUR == ':') {
	    xmlChar *tmp;

            xmlNsErr(ctxt, XML_NS_ERR_QNAME,
	             "Failed to parse QName '%s:%s:'\n", p, l, NULL);
	    NEXT;
	    tmp = (xmlChar *) xmlParseName(ctxt);
	    if (tmp != NULL) {
	        tmp = xmlBuildQName(tmp, l, NULL, 0);
		l = xmlDictLookup(ctxt->dict, tmp, -1);
		if (tmp != NULL) xmlFree(tmp);
		*prefix = p;
		return(l);
	    }
            if (ctxt->instate == XML_PARSER_EOF)
                return(NULL);
	    tmp = xmlBuildQName(BAD_CAST "", l, NULL, 0);
	    l = xmlDictLookup(ctxt->dict, tmp, -1);
	    if (tmp != NULL) xmlFree(tmp);
	    *prefix = p;
	    return(l);
	}
	*prefix = p;
    } else
        *prefix = NULL;
    return(l);
}