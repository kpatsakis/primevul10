rfbNewUDPClient(rfbScreenInfoPtr rfbScreen)
{
  return((rfbScreen->udpClient=
	  rfbNewTCPOrUDPClient(rfbScreen,rfbScreen->udpSock,TRUE)));
}