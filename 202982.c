rfbClientConnectionGone(rfbClientPtr cl)
{
    int i;

    LOCK(rfbClientListMutex);

    if (cl->prev)
        cl->prev->next = cl->next;
    else
        cl->screen->rfbClientHead = cl->next;
    if (cl->next)
        cl->next->prev = cl->prev;

    if(cl->sock>0)
        close(cl->sock);

#ifdef VINO_HAVE_ZLIB
    FreeZrleData(cl);
#endif

    if(cl->sock>=0)
       FD_CLR(cl->sock,&(cl->screen->allFds));

    if (cl->clientGoneHook)
      cl->clientGoneHook(cl);

    rfbLog("Client %s gone\n",cl->host);
    free(cl->host);

#ifdef VINO_HAVE_ZLIB
    /* Release the compression state structures if any. */
    if ( cl->compStreamInited ) {
	deflateEnd( &(cl->compStream) );
    }

#ifdef VINO_HAVE_JPEG
    for (i = 0; i < 4; i++) {
	if (cl->zsActive[i])
	    deflateEnd(&cl->zsStruct[i]);
    }
#endif
#endif

    if (pointerClient == cl)
        pointerClient = NULL;

    sraRgnDestroy(cl->modifiedRegion);
    sraRgnDestroy(cl->requestedRegion);
    sraRgnDestroy(cl->copyRegion);

    UNLOCK(rfbClientListMutex);

    if (cl->translateLookupTable) free(cl->translateLookupTable);

    rfbPrintStats(cl);

    free(cl);
}