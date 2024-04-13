xmlCheckCdataPush(const xmlChar *utf, int len, int complete) {
    int ix;
    unsigned char c;
    int codepoint;

    if ((utf == NULL) || (len <= 0))
        return(0);

    for (ix = 0; ix < len;) {      /* string is 0-terminated */
        c = utf[ix];
        if ((c & 0x80) == 0x00) {	/* 1-byte code, starts with 10 */
	    if (c >= 0x20)
		ix++;
	    else if ((c == 0xA) || (c == 0xD) || (c == 0x9))
	        ix++;
	    else
	        return(-ix);
	} else if ((c & 0xe0) == 0xc0) {/* 2-byte code, starts with 110 */
	    if (ix + 2 > len) return(complete ? -ix : ix);
	    if ((utf[ix+1] & 0xc0 ) != 0x80)
	        return(-ix);
	    codepoint = (utf[ix] & 0x1f) << 6;
	    codepoint |= utf[ix+1] & 0x3f;
	    if (!xmlIsCharQ(codepoint))
	        return(-ix);
	    ix += 2;
	} else if ((c & 0xf0) == 0xe0) {/* 3-byte code, starts with 1110 */
	    if (ix + 3 > len) return(complete ? -ix : ix);
	    if (((utf[ix+1] & 0xc0) != 0x80) ||
	        ((utf[ix+2] & 0xc0) != 0x80))
		    return(-ix);
	    codepoint = (utf[ix] & 0xf) << 12;
	    codepoint |= (utf[ix+1] & 0x3f) << 6;
	    codepoint |= utf[ix+2] & 0x3f;
	    if (!xmlIsCharQ(codepoint))
	        return(-ix);
	    ix += 3;
	} else if ((c & 0xf8) == 0xf0) {/* 4-byte code, starts with 11110 */
	    if (ix + 4 > len) return(complete ? -ix : ix);
	    if (((utf[ix+1] & 0xc0) != 0x80) ||
	        ((utf[ix+2] & 0xc0) != 0x80) ||
		((utf[ix+3] & 0xc0) != 0x80))
		    return(-ix);
	    codepoint = (utf[ix] & 0x7) << 18;
	    codepoint |= (utf[ix+1] & 0x3f) << 12;
	    codepoint |= (utf[ix+2] & 0x3f) << 6;
	    codepoint |= utf[ix+3] & 0x3f;
	    if (!xmlIsCharQ(codepoint))
	        return(-ix);
	    ix += 4;
	} else				/* unknown encoding */
	    return(-ix);
      }
      return(ix);
}