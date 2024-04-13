xmlAddSpecialAttr(xmlParserCtxtPtr ctxt,
		  const xmlChar *fullname,
		  const xmlChar *fullattr,
		  int type)
{
    if (ctxt->attsSpecial == NULL) {
        ctxt->attsSpecial = xmlHashCreateDict(10, ctxt->dict);
	if (ctxt->attsSpecial == NULL)
	    goto mem_error;
    }

    if (xmlHashLookup2(ctxt->attsSpecial, fullname, fullattr) != NULL)
        return;

    xmlHashAddEntry2(ctxt->attsSpecial, fullname, fullattr,
                     (void *) (ptrdiff_t) type);
    return;

mem_error:
    xmlErrMemory(ctxt, NULL);
    return;
}