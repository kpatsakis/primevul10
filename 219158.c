rfbSetProtocolVersion(rfbScreenInfoPtr rfbScreen, int major_, int minor_)
{
    /* Permit the server to set the version to report */
    /* TODO: sanity checking */
    if ((major_==3) && (minor_ > 2 && minor_ < 9))
    {
      rfbScreen->protocolMajorVersion = major_;
      rfbScreen->protocolMinorVersion = minor_;
    }
    else
        rfbLog("rfbSetProtocolVersion(%d,%d) set to invalid values\n", major_, minor_);
}