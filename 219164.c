rfbSendExtDesktopSize(rfbClientPtr cl,
                 int w,
                 int h)
{
    rfbFramebufferUpdateRectHeader rect;
    rfbExtDesktopSizeMsg edsHdr;
    rfbExtDesktopScreen eds;
    int i;
    char *logmsg;
    int numScreens = cl->screen->numberOfExtDesktopScreensHook(cl);

    if (cl->ublen + sz_rfbFramebufferUpdateRectHeader
            + sz_rfbExtDesktopSizeMsg
            + sz_rfbExtDesktopScreen * numScreens > UPDATE_BUF_SIZE) {
        if (!rfbSendUpdateBuf(cl))
            return FALSE;
    }

    rect.encoding = Swap32IfLE(rfbEncodingExtDesktopSize);
    rect.r.w = Swap16IfLE(w);
    rect.r.h = Swap16IfLE(h);
    rect.r.x = Swap16IfLE(cl->requestedDesktopSizeChange);
    rect.r.y = Swap16IfLE(cl->lastDesktopSizeChangeError);

    logmsg = "";

    if (cl->requestedDesktopSizeChange == rfbExtDesktopSize_ClientRequestedChange)
    {
        /* our client requested the resize through setDesktopSize */

        switch (cl->lastDesktopSizeChangeError)
        {
        case rfbExtDesktopSize_Success:
            logmsg = "resize successful";
            break;
        case rfbExtDesktopSize_ResizeProhibited:
            logmsg = "resize prohibited";
            break;
        case rfbExtDesktopSize_OutOfResources:
            logmsg = "resize failed: out of resources";
            break;
        case rfbExtDesktopSize_InvalidScreenLayout:
            logmsg = "resize failed: invalid screen layout";
            break;
        default:
            break;
        }
    }

    cl->requestedDesktopSizeChange = 0;
    cl->lastDesktopSizeChangeError = 0;

    rfbLog("Sending rfbEncodingExtDesktopSize for size (%dx%d) %s\n", w, h, logmsg);

    memcpy(&cl->updateBuf[cl->ublen], (char *)&rect,
           sz_rfbFramebufferUpdateRectHeader);
    cl->ublen += sz_rfbFramebufferUpdateRectHeader;

    edsHdr.numberOfScreens = numScreens;
    edsHdr.pad[0] = edsHdr.pad[1] = edsHdr.pad[2] = 0;
    memcpy(&cl->updateBuf[cl->ublen], (char *)&edsHdr,
           sz_rfbExtDesktopSizeMsg);
    cl->ublen += sz_rfbExtDesktopSizeMsg;

    for (i=0; i<numScreens; i++) {
        if (!cl->screen->getExtDesktopScreenHook(i, &eds, cl))
        {
            rfbErr("Error getting ExtendedDesktopSize information for screen #%d\n", i);
            return FALSE;
        }
        eds.id = Swap32IfLE(eds.id);
        eds.x = Swap16IfLE(eds.x);
        eds.y = Swap16IfLE(eds.y);
        eds.width = Swap16IfLE(eds.width);
        eds.height = Swap16IfLE(eds.height);
        eds.flags = Swap32IfLE(eds.flags);
        memcpy(&cl->updateBuf[cl->ublen], (char *)&eds,
               sz_rfbExtDesktopScreen);
        cl->ublen += sz_rfbExtDesktopScreen;
    }

    rfbStatRecordEncodingSent(cl, rfbEncodingExtDesktopSize,
                              sz_rfbFramebufferUpdateRectHeader + sz_rfbExtDesktopSizeMsg + sz_rfbExtDesktopScreen * numScreens,
                              sz_rfbFramebufferUpdateRectHeader + sz_rfbExtDesktopSizeMsg + sz_rfbExtDesktopScreen * numScreens);

    return TRUE;
}