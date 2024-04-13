rpmRC hdrblobGet(hdrblob blob, uint32_t tag, rpmtd td)
{
    rpmRC rc = RPMRC_NOTFOUND;
    struct indexEntry_s entry;
    struct entryInfo_s einfo;
    const struct entryInfo_s *pe = blob->pe;
    uint32_t ntag = htonl(tag);
    int tsize;

    memset(&einfo, 0, sizeof(einfo));
    rpmtdReset(td);

    for (int i = 0; i < blob->il; i++, pe++) {
	if (pe->tag != ntag)
	    continue;
	ei2h(pe, &einfo);

	/* We can only handle non-byteswappable data */
	tsize = typeSizes[einfo.type];
	if (tsize != 1 && tsize != -1)
	    return RPMRC_FAIL;

	entry.info = einfo; /* struct assignment */
	entry.data = blob->dataStart + einfo.offset;
	entry.length = dataLength(einfo.type, blob->dataStart + einfo.offset,
			 einfo.count, 1, blob->dataEnd);
	entry.rdlen = 0;
	td->tag = einfo.tag;
	rc = copyTdEntry(&entry, td, HEADERGET_MINMEM) ? RPMRC_OK : RPMRC_FAIL;
	break;
    }
    return rc;
}