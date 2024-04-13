rfbProcessClientInitMessage(rfbClientPtr cl)
{
    rfbClientInitMsg ci;
    union {
        char buf[256];
        rfbServerInitMsg si;
    } u;
    int len, n;
    rfbClientIteratorPtr iterator;
    rfbClientPtr otherCl;
    rfbExtensionData* extension;

    if (cl->state == RFB_INITIALISATION_SHARED) {
        /* In this case behave as though an implicit ClientInit message has
         * already been received with a shared-flag of true. */
        ci.shared = 1;
        /* Avoid the possibility of exposing the RFB_INITIALISATION_SHARED
         * state to calling software. */
        cl->state = RFB_INITIALISATION;
    } else {
        if ((n = rfbReadExact(cl, (char *)&ci,sz_rfbClientInitMsg)) <= 0) {
            if (n == 0)
                rfbLog("rfbProcessClientInitMessage: client gone\n");
            else
                rfbLogPerror("rfbProcessClientInitMessage: read");
            rfbCloseClient(cl);
            return;
        }
    }

    memset(u.buf,0,sizeof(u.buf));

    u.si.framebufferWidth = Swap16IfLE(cl->screen->width);
    u.si.framebufferHeight = Swap16IfLE(cl->screen->height);
    u.si.format = cl->screen->serverFormat;
    u.si.format.redMax = Swap16IfLE(u.si.format.redMax);
    u.si.format.greenMax = Swap16IfLE(u.si.format.greenMax);
    u.si.format.blueMax = Swap16IfLE(u.si.format.blueMax);

    strncpy(u.buf + sz_rfbServerInitMsg, cl->screen->desktopName, 127);
    len = strlen(u.buf + sz_rfbServerInitMsg);
    u.si.nameLength = Swap32IfLE(len);

    if (rfbWriteExact(cl, u.buf, sz_rfbServerInitMsg + len) < 0) {
        rfbLogPerror("rfbProcessClientInitMessage: write");
        rfbCloseClient(cl);
        return;
    }

    for(extension = cl->extensions; extension;) {
	rfbExtensionData* next = extension->next;
	if(extension->extension->init &&
		!extension->extension->init(cl, extension->data))
	    /* extension requested that it be removed */
	    rfbDisableExtension(cl, extension->extension);
	extension = next;
    }

    cl->state = RFB_NORMAL;

    if (!cl->reverseConnection &&
                        (cl->screen->neverShared || (!cl->screen->alwaysShared && !ci.shared))) {

        if (cl->screen->dontDisconnect) {
            iterator = rfbGetClientIterator(cl->screen);
            while ((otherCl = rfbClientIteratorNext(iterator)) != NULL) {
                if ((otherCl != cl) && (otherCl->state == RFB_NORMAL)) {
                    rfbLog("-dontdisconnect: Not shared & existing client\n");
                    rfbLog("  refusing new client %s\n", cl->host);
                    rfbCloseClient(cl);
                    rfbReleaseClientIterator(iterator);
                    return;
                }
            }
            rfbReleaseClientIterator(iterator);
        } else {
            iterator = rfbGetClientIterator(cl->screen);
            while ((otherCl = rfbClientIteratorNext(iterator)) != NULL) {
                if ((otherCl != cl) && (otherCl->state == RFB_NORMAL)) {
                    rfbLog("Not shared - closing connection to client %s\n",
                           otherCl->host);
                    rfbCloseClient(otherCl);
                }
            }
            rfbReleaseClientIterator(iterator);
        }
    }
}