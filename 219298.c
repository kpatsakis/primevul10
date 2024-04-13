xmlParseAttribute2(xmlParserCtxtPtr ctxt,
                   const xmlChar * pref, const xmlChar * elem,
                   const xmlChar ** prefix, xmlChar ** value,
                   int *len, int *alloc)
{
    const xmlChar *name;
    xmlChar *val, *internal_val = NULL;
    int normalize = 0;

    *value = NULL;
    GROW;
    name = xmlParseQName(ctxt, prefix);
    if (name == NULL) {
        xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
                       "error parsing attribute name\n");
        return (NULL);
    }

    /*
     * get the type if needed
     */
    if (ctxt->attsSpecial != NULL) {
        int type;

        type = (int) (ptrdiff_t) xmlHashQLookup2(ctxt->attsSpecial,
                                                 pref, elem, *prefix, name);
        if (type != 0)
            normalize = 1;
    }

    /*
     * read the value
     */
    SKIP_BLANKS;
    if (RAW == '=') {
        NEXT;
        SKIP_BLANKS;
        val = xmlParseAttValueInternal(ctxt, len, alloc, normalize);
	if (normalize) {
	    /*
	     * Sometimes a second normalisation pass for spaces is needed
	     * but that only happens if charrefs or entities refernces
	     * have been used in the attribute value, i.e. the attribute
	     * value have been extracted in an allocated string already.
	     */
	    if (*alloc) {
	        const xmlChar *val2;

	        val2 = xmlAttrNormalizeSpace2(ctxt, val, len);
		if ((val2 != NULL) && (val2 != val)) {
		    xmlFree(val);
		    val = (xmlChar *) val2;
		}
	    }
	}
        ctxt->instate = XML_PARSER_CONTENT;
    } else {
        xmlFatalErrMsgStr(ctxt, XML_ERR_ATTRIBUTE_WITHOUT_VALUE,
                          "Specification mandates value for attribute %s\n",
                          name);
        return (NULL);
    }

    if (*prefix == ctxt->str_xml) {
        /*
         * Check that xml:lang conforms to the specification
         * No more registered as an error, just generate a warning now
         * since this was deprecated in XML second edition
         */
        if ((ctxt->pedantic) && (xmlStrEqual(name, BAD_CAST "lang"))) {
            internal_val = xmlStrndup(val, *len);
            if (!xmlCheckLanguageID(internal_val)) {
                xmlWarningMsg(ctxt, XML_WAR_LANG_VALUE,
                              "Malformed value for xml:lang : %s\n",
                              internal_val, NULL);
            }
        }

        /*
         * Check that xml:space conforms to the specification
         */
        if (xmlStrEqual(name, BAD_CAST "space")) {
            internal_val = xmlStrndup(val, *len);
            if (xmlStrEqual(internal_val, BAD_CAST "default"))
                *(ctxt->space) = 0;
            else if (xmlStrEqual(internal_val, BAD_CAST "preserve"))
                *(ctxt->space) = 1;
            else {
                xmlWarningMsg(ctxt, XML_WAR_SPACE_VALUE,
                              "Invalid value \"%s\" for xml:space : \"default\" or \"preserve\" expected\n",
                              internal_val, NULL);
            }
        }
        if (internal_val) {
            xmlFree(internal_val);
        }
    }

    *value = val;
    return (name);
}