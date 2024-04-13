static bool jsi_wsAddStdHeader(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, struct lws *wsi,    Jsi_DString *hStr) {
    uchar buffer[JSI_BUFSIZ];
    uchar *p = (unsigned char *)buffer, *end = p + sizeof(buffer);
    const char *srv = cmdPtr->server;
    if (!srv) srv = "jsiWebSocket";
    int n = 0;
    if (srv[0] && lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_SERVER, (uchar*)srv, Jsi_Strlen(srv), &p, end))
        return false;
    n = p - buffer;
    if (n>0) {
        Jsi_DSAppendLen(hStr, (char*)buffer, n);
        p = buffer;
    }
    return true;
}