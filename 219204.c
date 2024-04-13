rfbSendBell(rfbScreenInfoPtr rfbScreen)
{
    rfbClientIteratorPtr i;
    rfbClientPtr cl;
    rfbBellMsg b;

    i = rfbGetClientIterator(rfbScreen);
    while((cl=rfbClientIteratorNext(i))) {
	b.type = rfbBell;
        LOCK(cl->sendMutex);
	if (rfbWriteExact(cl, (char *)&b, sz_rfbBellMsg) < 0) {
	    rfbLogPerror("rfbSendBell: write");
	    rfbCloseClient(cl);
	}
        UNLOCK(cl->sendMutex);
    }
    rfbStatRecordMessageSent(cl, rfbBell, sz_rfbBellMsg, sz_rfbBellMsg);
    rfbReleaseClientIterator(i);
}