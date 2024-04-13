rfbSendServerIdentity(rfbClientPtr cl)
{
    rfbFramebufferUpdateRectHeader rect;
    char buffer[512];

    /* tack on our library version */
    snprintf(buffer,sizeof(buffer)-1, "%s (%s)", 
        (cl->screen->versionString==NULL ? "unknown" : cl->screen->versionString),
        LIBVNCSERVER_PACKAGE_STRING);

    if (cl->ublen + sz_rfbFramebufferUpdateRectHeader
                  + (strlen(buffer)+1) > UPDATE_BUF_SIZE) {
        if (!rfbSendUpdateBuf(cl))
            return FALSE;
    }

    rect.encoding = Swap32IfLE(rfbEncodingServerIdentity);
    rect.r.x = 0;
    rect.r.y = 0;
    rect.r.w = Swap16IfLE(strlen(buffer)+1);
    rect.r.h = 0;

    memcpy(&cl->updateBuf[cl->ublen], (char *)&rect,
        sz_rfbFramebufferUpdateRectHeader);
    cl->ublen += sz_rfbFramebufferUpdateRectHeader;

    memcpy(&cl->updateBuf[cl->ublen], buffer, strlen(buffer)+1);
    cl->ublen += strlen(buffer)+1;

    rfbStatRecordEncodingSent(cl, rfbEncodingServerIdentity,
        sz_rfbFramebufferUpdateRectHeader+strlen(buffer)+1,
        sz_rfbFramebufferUpdateRectHeader+strlen(buffer)+1);
    

    if (!rfbSendUpdateBuf(cl))
        return FALSE;

    return TRUE;
}