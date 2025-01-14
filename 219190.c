rfbSendNewFBSize(rfbClientPtr cl,
                 int w,
                 int h)
{
    rfbFramebufferUpdateRectHeader rect;

    if (cl->ublen + sz_rfbFramebufferUpdateRectHeader > UPDATE_BUF_SIZE) {
	if (!rfbSendUpdateBuf(cl))
	    return FALSE;
    }

    if (cl->PalmVNC==TRUE)
        rfbLog("Sending rfbEncodingNewFBSize in response to a PalmVNC style framebuffer resize (%dx%d)\n", w, h);
    else
        rfbLog("Sending rfbEncodingNewFBSize for resize to (%dx%d)\n", w, h);

    rect.encoding = Swap32IfLE(rfbEncodingNewFBSize);
    rect.r.x = 0;
    rect.r.y = 0;
    rect.r.w = Swap16IfLE(w);
    rect.r.h = Swap16IfLE(h);

    memcpy(&cl->updateBuf[cl->ublen], (char *)&rect,
           sz_rfbFramebufferUpdateRectHeader);
    cl->ublen += sz_rfbFramebufferUpdateRectHeader;

    rfbStatRecordEncodingSent(cl, rfbEncodingNewFBSize, sz_rfbFramebufferUpdateRectHeader, sz_rfbFramebufferUpdateRectHeader);

    return TRUE;
}