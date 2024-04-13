rfbSendUpdateBuf(rfbClientPtr cl)
{
    if(cl->sock<0)
      return FALSE;

    if (rfbWriteExact(cl, cl->updateBuf, cl->ublen) < 0) {
        rfbLogPerror("rfbSendUpdateBuf: write");
        rfbCloseClient(cl);
        return FALSE;
    }

    cl->ublen = 0;
    return TRUE;
}