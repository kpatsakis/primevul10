xmlHaltParser(xmlParserCtxtPtr ctxt) {
    if (ctxt == NULL)
        return;
    ctxt->instate = XML_PARSER_EOF;
    ctxt->disableSAX = 1;
    while (ctxt->inputNr > 1)
        xmlFreeInputStream(inputPop(ctxt));
    if (ctxt->input != NULL) {
        /*
	 * in case there was a specific allocation deallocate before
	 * overriding base
	 */
        if (ctxt->input->free != NULL) {
	    ctxt->input->free((xmlChar *) ctxt->input->base);
	    ctxt->input->free = NULL;
	}
        if (ctxt->input->buf != NULL) {
            xmlFreeParserInputBuffer(ctxt->input->buf);
            ctxt->input->buf = NULL;
        }
	ctxt->input->cur = BAD_CAST"";
        ctxt->input->length = 0;
	ctxt->input->base = ctxt->input->cur;
        ctxt->input->end = ctxt->input->cur;
    }
}