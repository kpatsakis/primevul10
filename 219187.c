char *rfbProcessFileTransferReadBuffer(rfbClientPtr cl, uint32_t length)
{
    char *buffer=NULL;
    int   n=0;

    FILEXFER_ALLOWED_OR_CLOSE_AND_RETURN("", cl, NULL);

    /*
       We later alloc length+1, which might wrap around on 32-bit systems if length equals
       0XFFFFFFFF, i.e. SIZE_MAX for 32-bit systems. On 64-bit systems, a length of 0XFFFFFFFF
       will safely be allocated since this check will never trigger and malloc() can digest length+1
       without problems as length is a uint32_t.
       We also later pass length to rfbReadExact() that expects a signed int type and
       that might wrap on platforms with a 32-bit int type if length is bigger
       than 0X7FFFFFFF.
    */
    if(length == SIZE_MAX || length > INT_MAX) {
	rfbErr("rfbProcessFileTransferReadBuffer: too big file transfer length requested: %u", (unsigned int)length);
	rfbCloseClient(cl);
	return NULL;
    }

    if (length>0) {
        buffer=malloc((size_t)length+1);
        if (buffer!=NULL) {
            if ((n = rfbReadExact(cl, (char *)buffer, length)) <= 0) {
                if (n != 0)
                    rfbLogPerror("rfbProcessFileTransferReadBuffer: read");
                rfbCloseClient(cl);
                /* NOTE: don't forget to free(buffer) if you return early! */
                if (buffer!=NULL) free(buffer);
                return NULL;
            }
            /* Null Terminate */
            buffer[length]=0;
        }
    }
    return buffer;
}