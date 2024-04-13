int headerPut(Header h, rpmtd td, headerPutFlags flags)
{
    int rc;
    
    assert(td != NULL);
    if (flags & HEADERPUT_APPEND) {
	rc = findEntry(h, td->tag, td->type) ?
		intAppendEntry(h, td) :
		intAddEntry(h, td);
    } else {
	rc = intAddEntry(h, td);
    }
    return rc;
}