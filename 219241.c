int rfbSendNewScaleSize(rfbClientPtr cl)
{
    /* if the client supports newFBsize Encoding, use it */
    if (cl->useNewFBSize && cl->newFBSizePending)
	return FALSE;

    LOCK(cl->updateMutex);
    cl->newFBSizePending = FALSE;
    UNLOCK(cl->updateMutex);

    if (cl->PalmVNC==TRUE)
    {
        rfbPalmVNCReSizeFrameBufferMsg pmsg;
        pmsg.type = rfbPalmVNCReSizeFrameBuffer;
        pmsg.pad1 = 0;
        pmsg.desktop_w = Swap16IfLE(cl->screen->width);
        pmsg.desktop_h = Swap16IfLE(cl->screen->height);
        pmsg.buffer_w  = Swap16IfLE(cl->scaledScreen->width);
        pmsg.buffer_h  = Swap16IfLE(cl->scaledScreen->height);
        pmsg.pad2 = 0;

        rfbLog("Sending a response to a PalmVNC style frameuffer resize event (%dx%d)\n", cl->scaledScreen->width, cl->scaledScreen->height);
        if (rfbWriteExact(cl, (char *)&pmsg, sz_rfbPalmVNCReSizeFrameBufferMsg) < 0) {
            rfbLogPerror("rfbNewClient: write");
            rfbCloseClient(cl);
            return FALSE;
        }
    }
    else
    {
        rfbResizeFrameBufferMsg        rmsg;
        rmsg.type = rfbResizeFrameBuffer;
        rmsg.pad1=0;
        rmsg.framebufferWidth  = Swap16IfLE(cl->scaledScreen->width);
        rmsg.framebufferHeigth = Swap16IfLE(cl->scaledScreen->height);
        rfbLog("Sending a response to a UltraVNC style frameuffer resize event (%dx%d)\n", cl->scaledScreen->width, cl->scaledScreen->height);
        if (rfbWriteExact(cl, (char *)&rmsg, sz_rfbResizeFrameBufferMsg) < 0) {
            rfbLogPerror("rfbNewClient: write");
            rfbCloseClient(cl);
            return FALSE;
        }
    }
    return TRUE;
}