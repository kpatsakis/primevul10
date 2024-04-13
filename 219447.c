xmlStringLenDecodeEntities(xmlParserCtxtPtr ctxt, const xmlChar *str, int len,
		      int what, xmlChar end, xmlChar  end2, xmlChar end3) {
    xmlChar *buffer = NULL;
    size_t buffer_size = 0;
    size_t nbchars = 0;

    xmlChar *current = NULL;
    xmlChar *rep = NULL;
    const xmlChar *last;
    xmlEntityPtr ent;
    int c,l;

    if ((ctxt == NULL) || (str == NULL) || (len < 0))
	return(NULL);
    last = str + len;

    if (((ctxt->depth > 40) &&
         ((ctxt->options & XML_PARSE_HUGE) == 0)) ||
	(ctxt->depth > 1024)) {
	xmlFatalErr(ctxt, XML_ERR_ENTITY_LOOP, NULL);
	return(NULL);
    }

    /*
     * allocate a translation buffer.
     */
    buffer_size = XML_PARSER_BIG_BUFFER_SIZE;
    buffer = (xmlChar *) xmlMallocAtomic(buffer_size);
    if (buffer == NULL) goto mem_error;

    /*
     * OK loop until we reach one of the ending char or a size limit.
     * we are operating on already parsed values.
     */
    if (str < last)
	c = CUR_SCHAR(str, l);
    else
        c = 0;
    while ((c != 0) && (c != end) && /* non input consuming loop */
	   (c != end2) && (c != end3)) {

	if (c == 0) break;
        if ((c == '&') && (str[1] == '#')) {
	    int val = xmlParseStringCharRef(ctxt, &str);
	    if (val != 0) {
		COPY_BUF(0,buffer,nbchars,val);
	    }
	    if (nbchars + XML_PARSER_BUFFER_SIZE > buffer_size) {
	        growBuffer(buffer, XML_PARSER_BUFFER_SIZE);
	    }
	} else if ((c == '&') && (what & XML_SUBSTITUTE_REF)) {
	    if (xmlParserDebugEntities)
		xmlGenericError(xmlGenericErrorContext,
			"String decoding Entity Reference: %.30s\n",
			str);
	    ent = xmlParseStringEntityRef(ctxt, &str);
	    if ((ctxt->lastError.code == XML_ERR_ENTITY_LOOP) ||
	        (ctxt->lastError.code == XML_ERR_INTERNAL_ERROR))
	        goto int_error;
	    xmlParserEntityCheck(ctxt, 0, ent, 0);
	    if (ent != NULL)
	        ctxt->nbentities += ent->checked / 2;
	    if ((ent != NULL) &&
		(ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {
		if (ent->content != NULL) {
		    COPY_BUF(0,buffer,nbchars,ent->content[0]);
		    if (nbchars + XML_PARSER_BUFFER_SIZE > buffer_size) {
			growBuffer(buffer, XML_PARSER_BUFFER_SIZE);
		    }
		} else {
		    xmlFatalErrMsg(ctxt, XML_ERR_INTERNAL_ERROR,
			    "predefined entity has no content\n");
		}
	    } else if ((ent != NULL) && (ent->content != NULL)) {
		ctxt->depth++;
		rep = xmlStringDecodeEntities(ctxt, ent->content, what,
			                      0, 0, 0);
		ctxt->depth--;

		if ((ctxt->lastError.code == XML_ERR_ENTITY_LOOP) ||
		    (ctxt->lastError.code == XML_ERR_INTERNAL_ERROR))
		    goto int_error;

		if (rep != NULL) {
		    current = rep;
		    while (*current != 0) { /* non input consuming loop */
			buffer[nbchars++] = *current++;
			if (nbchars + XML_PARSER_BUFFER_SIZE > buffer_size) {
			    if (xmlParserEntityCheck(ctxt, nbchars, ent, 0))
				goto int_error;
			    growBuffer(buffer, XML_PARSER_BUFFER_SIZE);
			}
		    }
		    xmlFree(rep);
		    rep = NULL;
		}
	    } else if (ent != NULL) {
		int i = xmlStrlen(ent->name);
		const xmlChar *cur = ent->name;

		buffer[nbchars++] = '&';
		if (nbchars + i + XML_PARSER_BUFFER_SIZE > buffer_size) {
		    growBuffer(buffer, i + XML_PARSER_BUFFER_SIZE);
		}
		for (;i > 0;i--)
		    buffer[nbchars++] = *cur++;
		buffer[nbchars++] = ';';
	    }
	} else if (c == '%' && (what & XML_SUBSTITUTE_PEREF)) {
	    if (xmlParserDebugEntities)
		xmlGenericError(xmlGenericErrorContext,
			"String decoding PE Reference: %.30s\n", str);
	    ent = xmlParseStringPEReference(ctxt, &str);
	    if (ctxt->lastError.code == XML_ERR_ENTITY_LOOP)
	        goto int_error;
	    xmlParserEntityCheck(ctxt, 0, ent, 0);
	    if (ent != NULL)
	        ctxt->nbentities += ent->checked / 2;
	    if (ent != NULL) {
                if (ent->content == NULL) {
		    /*
		     * Note: external parsed entities will not be loaded,
		     * it is not required for a non-validating parser to
		     * complete external PEreferences coming from the
		     * internal subset
		     */
		    if (((ctxt->options & XML_PARSE_NOENT) != 0) ||
			((ctxt->options & XML_PARSE_DTDVALID) != 0) ||
			(ctxt->validate != 0)) {
			xmlLoadEntityContent(ctxt, ent);
		    } else {
			xmlWarningMsg(ctxt, XML_ERR_ENTITY_PROCESSING,
		  "not validating will not read content for PE entity %s\n",
		                      ent->name, NULL);
		    }
		}
		ctxt->depth++;
		rep = xmlStringDecodeEntities(ctxt, ent->content, what,
			                      0, 0, 0);
		ctxt->depth--;
		if (rep != NULL) {
		    current = rep;
		    while (*current != 0) { /* non input consuming loop */
			buffer[nbchars++] = *current++;
			if (nbchars + XML_PARSER_BUFFER_SIZE > buffer_size) {
			    if (xmlParserEntityCheck(ctxt, nbchars, ent, 0))
			        goto int_error;
			    growBuffer(buffer, XML_PARSER_BUFFER_SIZE);
			}
		    }
		    xmlFree(rep);
		    rep = NULL;
		}
	    }
	} else {
	    COPY_BUF(l,buffer,nbchars,c);
	    str += l;
	    if (nbchars + XML_PARSER_BUFFER_SIZE > buffer_size) {
	        growBuffer(buffer, XML_PARSER_BUFFER_SIZE);
	    }
	}
	if (str < last)
	    c = CUR_SCHAR(str, l);
	else
	    c = 0;
    }
    buffer[nbchars] = 0;
    return(buffer);

mem_error:
    xmlErrMemory(ctxt, NULL);
int_error:
    if (rep != NULL)
        xmlFree(rep);
    if (buffer != NULL)
        xmlFree(buffer);
    return(NULL);
}