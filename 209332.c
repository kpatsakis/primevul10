static int jsi_wsServeHeader(jsi_wsPss *pss, struct lws *wsi, int strLen,
    int code, const char *extra, const char *mime, Jsi_DString *jStr)
{
    uchar ubuf[JSI_BUFSIZ], *p=ubuf, *end = &ubuf[sizeof(ubuf)-1];
    if (!mime) mime = "text/html";
    if (code<=0) code = 200;
    if (lws_add_http_header_status(wsi, code, &p, end))
        return -1;
    const char *srv = pss->cmdPtr->server;
    if (!srv) srv = "jsiWebSocket";
    if (srv[0] && lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_SERVER, (uchar*)srv, Jsi_Strlen(srv), &p, end))
        return -1;
    if (lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_CONTENT_TYPE, (uchar *)mime, Jsi_Strlen(mime), &p, end))
        return -1;
    if (lws_add_http_header_content_length(wsi, strLen, &p, end))
        return -1;
    *p = 0;
    Jsi_DSAppend(jStr, (char*)ubuf, extra, NULL);
    p = ubuf;
    if (lws_finalize_http_header(wsi, &p, end))
        return -1;
    *p = 0;
    Jsi_DSAppend(jStr, (char*)ubuf, NULL);
    return 0;
}