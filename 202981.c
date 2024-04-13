rfbNewClient(rfbScreenInfoPtr rfbScreen,
             int sock)
{
    rfbProtocolVersionMsg pv;
    rfbClientIteratorPtr iterator;
    rfbClientPtr cl,cl_;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    int i;
    char host[NI_MAXHOST];
    const char *prt = "unknown";

    cl = (rfbClientPtr)calloc(sizeof(rfbClientRec),1);

    cl->screen = rfbScreen;
    cl->sock = sock;
    cl->viewOnly = FALSE;

    rfbResetStats(cl);

    {
      int one=1;

      getpeername(sock, (struct sockaddr *)&addr, &addrlen);

      getnameinfo((struct sockaddr *)&addr,
                  addrlen,
                  host, sizeof(host),
                  NULL, 0,
                  0);

      cl->host = strdup(host);

      if (addr.ss_family == AF_INET)
        prt = "IPv4";
      else if (addr.ss_family == AF_INET6)
        prt = "IPv6";

      rfbLog("[%s] Got connection from client %s\n", prt, cl->host);

      rfbLog("  other clients:\n");
      iterator = rfbGetClientIterator(rfbScreen);
      while ((cl_ = rfbClientIteratorNext(iterator)) != NULL) {
        rfbLog("     %s\n",cl_->host);
      }
      rfbReleaseClientIterator(iterator);

#ifndef WIN32
      if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
	rfbLogPerror("fcntl failed");
	close(sock);
	return NULL;
      }
#endif

      if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		     (char *)&one, sizeof(one)) < 0) {
	rfbLogPerror("setsockopt failed");
	close(sock);
	return NULL;
      }

      FD_SET(sock,&(rfbScreen->allFds));
		rfbScreen->maxFd = max(sock,rfbScreen->maxFd);

      cl->state = RFB_PROTOCOL_VERSION;

      cl->readyForSetColourMapEntries = FALSE;
      cl->useCopyRect = FALSE;
      cl->preferredEncoding = rfbEncodingRaw;
      cl->correMaxWidth = 48;
      cl->correMaxHeight = 48;
#ifdef VINO_HAVE_ZLIB
      cl->zrleData = NULL;
#endif

      cl->copyRegion = sraRgnCreate();
      cl->copyDX = 0;
      cl->copyDY = 0;
   
      cl->modifiedRegion =
	sraRgnCreateRect(0,0,rfbScreen->width,rfbScreen->height);

      cl->requestedRegion = sraRgnCreate();

      cl->format = cl->screen->rfbServerFormat;
      cl->translateFn = rfbTranslateNone;
      cl->translateLookupTable = NULL;

      LOCK(rfbClientListMutex);

      cl->next = rfbScreen->rfbClientHead;
      cl->prev = NULL;
      if (rfbScreen->rfbClientHead)
        rfbScreen->rfbClientHead->prev = cl;

      rfbScreen->rfbClientHead = cl;
      UNLOCK(rfbClientListMutex);

#ifdef VINO_HAVE_JPEG
      cl->tightCompressLevel = TIGHT_DEFAULT_COMPRESSION;
      cl->tightQualityLevel = -1;
      for (i = 0; i < 4; i++)
        cl->zsActive[i] = FALSE;
#endif

      cl->enableCursorUpdates = FALSE;
      cl->useRichCursorEncoding = FALSE;
      cl->enableLastRectEncoding = FALSE;
      cl->useNewFBSize = FALSE;

#ifdef VINO_HAVE_ZLIB
      cl->compStreamInited = FALSE;
      cl->compStream.total_in = 0;
      cl->compStream.total_out = 0;
      cl->compStream.zalloc = Z_NULL;
      cl->compStream.zfree = Z_NULL;
      cl->compStream.opaque = Z_NULL;

      cl->zlibCompressLevel = 5;
#endif

      sprintf(pv,rfbProtocolVersionFormat,rfbProtocolMajorVersion,
	      rfbProtocolMinorVersion7);

      if (WriteExact(cl, pv, sz_rfbProtocolVersionMsg) < 0) {
        rfbLogPerror("rfbNewClient: write");
        rfbCloseClient(cl);
        rfbClientConnectionGone(cl);
        return NULL;
      }
    }

    cl->clientData = NULL;
    cl->clientGoneHook = NULL;
    cl->onHold = FALSE;
    if (cl->screen->newClientHook) {
	switch (cl->screen->newClientHook(cl)) {
	case RFB_CLIENT_ON_HOLD:
	    cl->onHold = TRUE;
	    break;
	case RFB_CLIENT_ACCEPT:
	    break;
	case RFB_CLIENT_REFUSE:
	    rfbCloseClient(cl);
	    rfbClientConnectionGone(cl);
	    cl = NULL;
	    break;
	}
    }

    return cl;
}