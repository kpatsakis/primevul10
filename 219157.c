rfbNewClient(rfbScreenInfoPtr rfbScreen,
             rfbSocket sock)
{
  return(rfbNewTCPOrUDPClient(rfbScreen,sock,FALSE));
}