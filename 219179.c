rfbReverseConnection(rfbScreenInfoPtr rfbScreen,
                     char *host,
                     int port)
{
    rfbSocket sock;
    rfbClientPtr cl;

    if ((sock = rfbConnect(rfbScreen, host, port)) < 0)
        return (rfbClientPtr)NULL;

    cl = rfbNewClient(rfbScreen, sock);

    if (cl) {
        cl->reverseConnection = TRUE;
    }

    return cl;
}