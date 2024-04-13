static bool jsi_wsAddHeader(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, struct lws *wsi, Jsi_Value *hdrs,
    Jsi_DString *hStr) {
    uchar buffer[JSI_BUFSIZ];
    uchar *p = (unsigned char *)buffer, *end = p + sizeof(buffer);
    int n = 0;
    int i, hvl, argc = Jsi_ValueGetLength(interp, hdrs);
    for (i=0; i<argc; i+=2) {
        const char *hn = Jsi_ValueArrayIndexToStr(interp, hdrs, i, NULL),
            *hv = Jsi_ValueArrayIndexToStr(interp, hdrs, i+1, &hvl);
        if (hn && hv) {
            if (lws_add_http_header_by_name(wsi, (const uchar *)hn, (const uchar *)hv, hvl, &p, end))
                return false;
            n = p - buffer;
            if (n>0)
                Jsi_DSAppendLen(hStr, (char*)buffer, n);
            p = buffer;
        }
    }
    return true;
}