jsi_ws_http_redirect(struct lws *wsi, int code, Jsi_DString *tStr, 
                  unsigned char **p, unsigned char *end)
{
    char *loc = Jsi_DSValue(tStr);
    uchar *start = *p;
    char* cookie = Jsi_Strchr(loc, '|');
    if (cookie) { *cookie= 0; cookie++; }

    if (lws_add_http_header_status(wsi, code, p, end)
        || lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_LOCATION, (uchar *)loc, Jsi_Strlen(loc), p, end)
        || lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_CONTENT_TYPE,(uchar *)"text/html", 9, p,end)
        || lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_CONTENT_LENGTH, (uchar *)"0", 1, p, end))
        return -1;
    if (cookie && lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_SET_COOKIE, (uchar *)cookie, Jsi_Strlen(cookie), p, end))
        return -1;
    if (lws_finalize_http_header(wsi, p, end))
        return -1;
    return lws_write(wsi, start, *p - start, LWS_WRITE_HTTP_HEADERS);
}