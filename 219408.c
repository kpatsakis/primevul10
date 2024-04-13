xmlDumpEntityContent(xmlBufferPtr buf, const xmlChar *content) {
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return;
    if (xmlStrchr(content, '%')) {
        const xmlChar * base, *cur;

	xmlBufferCCat(buf, "\"");
	base = cur = content;
	while (*cur != 0) {
	    if (*cur == '"') {
		if (base != cur)
		    xmlBufferAdd(buf, base, cur - base);
		xmlBufferAdd(buf, BAD_CAST "&quot;", 6);
		cur++;
		base = cur;
	    } else if (*cur == '%') {
		if (base != cur)
		    xmlBufferAdd(buf, base, cur - base);
		xmlBufferAdd(buf, BAD_CAST "&#x25;", 6);
		cur++;
		base = cur;
	    } else {
		cur++;
	    }
	}
	if (base != cur)
	    xmlBufferAdd(buf, base, cur - base);
	xmlBufferCCat(buf, "\"");
    } else {
        xmlBufferWriteQuotedString(buf, content);
    }
}