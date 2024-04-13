xmlDumpEntityDecl(xmlBufferPtr buf, xmlEntityPtr ent) {
    if ((buf == NULL) || (ent == NULL)) return;
    switch (ent->etype) {
	case XML_INTERNAL_GENERAL_ENTITY:
	    xmlBufferWriteChar(buf, "<!ENTITY ");
	    xmlBufferWriteCHAR(buf, ent->name);
	    xmlBufferWriteChar(buf, " ");
	    if (ent->orig != NULL)
		xmlBufferWriteQuotedString(buf, ent->orig);
	    else
		xmlDumpEntityContent(buf, ent->content);
	    xmlBufferWriteChar(buf, ">\n");
	    break;
	case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
	    xmlBufferWriteChar(buf, "<!ENTITY ");
	    xmlBufferWriteCHAR(buf, ent->name);
	    if (ent->ExternalID != NULL) {
		 xmlBufferWriteChar(buf, " PUBLIC ");
		 xmlBufferWriteQuotedString(buf, ent->ExternalID);
		 xmlBufferWriteChar(buf, " ");
		 xmlBufferWriteQuotedString(buf, ent->SystemID);
	    } else {
		 xmlBufferWriteChar(buf, " SYSTEM ");
		 xmlBufferWriteQuotedString(buf, ent->SystemID);
	    }
	    xmlBufferWriteChar(buf, ">\n");
	    break;
	case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	    xmlBufferWriteChar(buf, "<!ENTITY ");
	    xmlBufferWriteCHAR(buf, ent->name);
	    if (ent->ExternalID != NULL) {
		 xmlBufferWriteChar(buf, " PUBLIC ");
		 xmlBufferWriteQuotedString(buf, ent->ExternalID);
		 xmlBufferWriteChar(buf, " ");
		 xmlBufferWriteQuotedString(buf, ent->SystemID);
	    } else {
		 xmlBufferWriteChar(buf, " SYSTEM ");
		 xmlBufferWriteQuotedString(buf, ent->SystemID);
	    }
	    if (ent->content != NULL) { /* Should be true ! */
		xmlBufferWriteChar(buf, " NDATA ");
		if (ent->orig != NULL)
		    xmlBufferWriteCHAR(buf, ent->orig);
		else
		    xmlBufferWriteCHAR(buf, ent->content);
	    }
	    xmlBufferWriteChar(buf, ">\n");
	    break;
	case XML_INTERNAL_PARAMETER_ENTITY:
	    xmlBufferWriteChar(buf, "<!ENTITY % ");
	    xmlBufferWriteCHAR(buf, ent->name);
	    xmlBufferWriteChar(buf, " ");
	    if (ent->orig == NULL)
		xmlDumpEntityContent(buf, ent->content);
	    else
		xmlBufferWriteQuotedString(buf, ent->orig);
	    xmlBufferWriteChar(buf, ">\n");
	    break;
	case XML_EXTERNAL_PARAMETER_ENTITY:
	    xmlBufferWriteChar(buf, "<!ENTITY % ");
	    xmlBufferWriteCHAR(buf, ent->name);
	    if (ent->ExternalID != NULL) {
		 xmlBufferWriteChar(buf, " PUBLIC ");
		 xmlBufferWriteQuotedString(buf, ent->ExternalID);
		 xmlBufferWriteChar(buf, " ");
		 xmlBufferWriteQuotedString(buf, ent->SystemID);
	    } else {
		 xmlBufferWriteChar(buf, " SYSTEM ");
		 xmlBufferWriteQuotedString(buf, ent->SystemID);
	    }
	    xmlBufferWriteChar(buf, ">\n");
	    break;
	default:
	    xmlEntitiesErr(XML_DTD_UNKNOWN_ENTITY,
		"xmlDumpEntitiesDecl: internal: unknown type entity type");
    }
}