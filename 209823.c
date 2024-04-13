static int jsi_wsServeString(jsi_wsPss *pss, struct lws *wsi,
    const char *buf, int code, const char *extra, const char *mime)
{
    int strLen = Jsi_Strlen(buf);
    Jsi_DString jStr = {};
    int rc = jsi_wsServeHeader(pss, wsi, strLen, code, extra, mime, &jStr);
    if (rc>=0) {
        Jsi_DSAppend(&jStr, buf, NULL);
        char *vStr = Jsi_DSValue(&jStr);
        int vLen = Jsi_DSLength(&jStr);
        rc = jsi_wswrite(pss, wsi, (unsigned char*)vStr, vLen, LWS_WRITE_HTTP);
    }
    Jsi_DSFree(&jStr);
    return (rc>=0?1:0);
}