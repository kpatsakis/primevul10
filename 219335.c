xmlPopInput(xmlParserCtxtPtr ctxt) {
    if ((ctxt == NULL) || (ctxt->inputNr <= 1)) return(0);
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"Popping input %d\n", ctxt->inputNr);
    if ((ctxt->inputNr > 1) && (ctxt->inSubset == 0) &&
        (ctxt->instate != XML_PARSER_EOF))
        xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
                    "Unfinished entity outside the DTD");
    xmlFreeInputStream(inputPop(ctxt));
    if (*ctxt->input->cur == 0)
        xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
    return(CUR);
}