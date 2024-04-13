xmlCleanSpecialAttrCallback(void *payload, void *data,
                            const xmlChar *fullname, const xmlChar *fullattr,
                            const xmlChar *unused ATTRIBUTE_UNUSED) {
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) data;

    if (((ptrdiff_t) payload) == XML_ATTRIBUTE_CDATA) {
        xmlHashRemoveEntry2(ctxt->attsSpecial, fullname, fullattr, NULL);
    }
}