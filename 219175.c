rfbSendXvp(rfbClientPtr cl, uint8_t version, uint8_t code)
{
    rfbXvpMsg xvp;

    xvp.type = rfbXvp;
    xvp.pad = 0;
    xvp.version = version;
    xvp.code = code;

    LOCK(cl->sendMutex);
    if (rfbWriteExact(cl, (char *)&xvp, sz_rfbXvpMsg) < 0) {
      rfbLogPerror("rfbSendXvp: write");
      rfbCloseClient(cl);
    }
    UNLOCK(cl->sendMutex);

    rfbStatRecordMessageSent(cl, rfbXvp, sz_rfbXvpMsg, sz_rfbXvpMsg);

    return TRUE;
}