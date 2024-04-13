xmlDocPtr soap_xmlParseMemory(const void *buf, size_t buf_size)
{
	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr ret;

	TSRMLS_FETCH();

/*
	xmlInitParser();
*/
	ctxt = xmlCreateMemoryParserCtxt(buf, buf_size);
	if (ctxt) {
		zend_bool old;

		ctxt->sax->ignorableWhitespace = soap_ignorableWhitespace;
		ctxt->sax->comment = soap_Comment;
		ctxt->sax->warning = NULL;
		ctxt->sax->error = NULL;
		/*ctxt->sax->fatalError = NULL;*/
#if LIBXML_VERSION >= 20703
		ctxt->options |= XML_PARSE_HUGE;
#endif
		old = php_libxml_disable_entity_loader(1 TSRMLS_CC);
		xmlParseDocument(ctxt);
		php_libxml_disable_entity_loader(old TSRMLS_CC);
		if (ctxt->wellFormed) {
			ret = ctxt->myDoc;
			if (ret->URL == NULL && ctxt->directory != NULL) {
				ret->URL = xmlCharStrdup(ctxt->directory);
			}
		} else {
			ret = NULL;
			xmlFreeDoc(ctxt->myDoc);
			ctxt->myDoc = NULL;
		}
		xmlFreeParserCtxt(ctxt);
	} else {
		ret = NULL;
	}

/*
	xmlCleanupParser();
*/

/*
	if (ret) {
		cleanup_xml_node((xmlNodePtr)ret);
	}
*/
	return ret;
}