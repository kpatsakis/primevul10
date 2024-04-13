xmlEncodeSpecialChars(const xmlDoc *doc ATTRIBUTE_UNUSED, const xmlChar *input) {
    const xmlChar *cur = input;
    xmlChar *buffer = NULL;
    xmlChar *out = NULL;
    size_t buffer_size = 0;
    if (input == NULL) return(NULL);

    /*
     * allocate an translation buffer.
     */
    buffer_size = 1000;
    buffer = (xmlChar *) xmlMalloc(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
        xmlEntitiesErrMemory("xmlEncodeSpecialChars: malloc failed");
	return(NULL);
    }
    out = buffer;

    while (*cur != '\0') {
        size_t indx = out - buffer;
        if (indx + 10 > buffer_size) {

	    growBufferReentrant();
	    out = &buffer[indx];
	}

	/*
	 * By default one have to encode at least '<', '>', '"' and '&' !
	 */
	if (*cur == '<') {
	    *out++ = '&';
	    *out++ = 'l';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '>') {
	    *out++ = '&';
	    *out++ = 'g';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '&') {
	    *out++ = '&';
	    *out++ = 'a';
	    *out++ = 'm';
	    *out++ = 'p';
	    *out++ = ';';
	} else if (*cur == '"') {
	    *out++ = '&';
	    *out++ = 'q';
	    *out++ = 'u';
	    *out++ = 'o';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '\r') {
	    *out++ = '&';
	    *out++ = '#';
	    *out++ = '1';
	    *out++ = '3';
	    *out++ = ';';
	} else {
	    /*
	     * Works because on UTF-8, all extended sequences cannot
	     * result in bytes in the ASCII range.
	     */
	    *out++ = *cur;
	}
	cur++;
    }
    *out = 0;
    return(buffer);

mem_error:
    xmlEntitiesErrMemory("xmlEncodeSpecialChars: realloc failed");
    xmlFree(buffer);
    return(NULL);
}