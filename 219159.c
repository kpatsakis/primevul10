rfbSendKeyboardLedState(rfbClientPtr cl)
{
    rfbFramebufferUpdateRectHeader rect;

    if (cl->ublen + sz_rfbFramebufferUpdateRectHeader > UPDATE_BUF_SIZE) {
        if (!rfbSendUpdateBuf(cl))
            return FALSE;
    }

    rect.encoding = Swap32IfLE(rfbEncodingKeyboardLedState);
    rect.r.x = Swap16IfLE(cl->lastKeyboardLedState);
    rect.r.y = 0;
    rect.r.w = 0;
    rect.r.h = 0;

    memcpy(&cl->updateBuf[cl->ublen], (char *)&rect,
        sz_rfbFramebufferUpdateRectHeader);
    cl->ublen += sz_rfbFramebufferUpdateRectHeader;

    rfbStatRecordEncodingSent(cl, rfbEncodingKeyboardLedState, sz_rfbFramebufferUpdateRectHeader, sz_rfbFramebufferUpdateRectHeader);

    if (!rfbSendUpdateBuf(cl))
        return FALSE;

    return TRUE;
}