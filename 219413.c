xmlEncodeEntitiesReentrant(xmlDocPtr doc, const xmlChar *input) {
    return xmlEncodeEntitiesInternal(doc, input, 0);
}