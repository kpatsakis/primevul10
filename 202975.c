rfbProcessClientProtocolVersion(rfbClientPtr cl)
{
    rfbProtocolVersionMsg pv;
    int n, major_, minor_;

    if ((n = ReadExact(cl, pv, sz_rfbProtocolVersionMsg)) <= 0) {
        if (n == 0)
            rfbLog("rfbProcessClientProtocolVersion: client gone\n");
        else
            rfbLogPerror("rfbProcessClientProtocolVersion: read");
        rfbCloseClient(cl);
        return;
    }

    pv[sz_rfbProtocolVersionMsg] = 0;
    if (sscanf(pv,rfbProtocolVersionFormat,&major_,&minor_) != 2) {
        char name[1024]; 
	if(sscanf(pv,"RFB %03d.%03d %1023s\n",&major_,&minor_,name) != 3) {
	    rfbErr("rfbProcessClientProtocolVersion: not a valid RFB client\n");
	    rfbCloseClient(cl);
	    return;
	}
	free(cl->host);
	cl->host=strdup(name);
    }
    rfbLog("Client Protocol Version %d.%d\n", major_, minor_);

    if (major_ != rfbProtocolMajorVersion) {
        rfbErr("RFB protocol version mismatch - server %d.%d, client %d.%d",
                rfbProtocolMajorVersion, rfbProtocolMinorVersion7,
                major_,minor_);
        rfbCloseClient(cl);
        return;
    }

    if (minor_ != rfbProtocolMinorVersion3 &&
	minor_ != rfbProtocolMinorVersion7 &&
	minor_ != rfbProtocolMinorVersion8) {
        /* Minor version mismatch - warn but try to continue */
        rfbLog("Ignoring minor version mismatch\n");
    }

    if (minor_ >= rfbProtocolMinorVersion8)
      cl->minorVersion = rfbProtocolMinorVersion8;
    else if (minor_ == rfbProtocolMinorVersion7)
      cl->minorVersion = rfbProtocolMinorVersion7;
    else
      cl->minorVersion = rfbProtocolMinorVersion3;

    rfbAuthNewClient(cl);
}