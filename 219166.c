rfbProcessClientProtocolVersion(rfbClientPtr cl)
{
    rfbProtocolVersionMsg pv;
    int n, major_, minor_;

    if ((n = rfbReadExact(cl, pv, sz_rfbProtocolVersionMsg)) <= 0) {
        if (n == 0)
            rfbLog("rfbProcessClientProtocolVersion: client gone\n");
        else
            rfbLogPerror("rfbProcessClientProtocolVersion: read");
        rfbCloseClient(cl);
        return;
    }

    pv[sz_rfbProtocolVersionMsg] = 0;
    if (sscanf(pv,rfbProtocolVersionFormat,&major_,&minor_) != 2) {
	rfbErr("rfbProcessClientProtocolVersion: not a valid RFB client: %s\n", pv);
	rfbCloseClient(cl);
	return;
    }
    rfbLog("Client Protocol Version %d.%d\n", major_, minor_);

    if (major_ != rfbProtocolMajorVersion) {
        rfbErr("RFB protocol version mismatch - server %d.%d, client %d.%d",
                cl->screen->protocolMajorVersion, cl->screen->protocolMinorVersion,
                major_,minor_);
        rfbCloseClient(cl);
        return;
    }

    /* Check for the minor version use either of the two standard version of RFB */
    /*
     * UltraVNC Viewer detects FileTransfer compatible servers via rfb versions
     * 3.4, 3.6, 3.14, 3.16
     * It's a bad method, but it is what they use to enable features...
     * maintaining RFB version compatibility across multiple servers is a pain
     * Should use something like ServerIdentity encoding
     */
    cl->protocolMajorVersion = major_;
    cl->protocolMinorVersion = minor_;
    
    rfbLog("Protocol version sent %d.%d, using %d.%d\n",
              major_, minor_, rfbProtocolMajorVersion, cl->protocolMinorVersion);

    rfbAuthNewClient(cl);
}