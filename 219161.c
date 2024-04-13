rfbNewUDPConnection(rfbScreenInfoPtr rfbScreen,
                    rfbSocket sock)
{
  if (write(sock, (char*) &ptrAcceleration, 1) < 0) {
	rfbLogPerror("rfbNewUDPConnection: write");
    }
}