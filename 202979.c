rfbNewClientConnection(rfbScreenInfoPtr rfbScreen,
                       int sock)
{
    rfbClientPtr cl;

    cl = rfbNewClient(rfbScreen,sock);
}