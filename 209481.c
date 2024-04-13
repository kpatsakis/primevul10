static void jsi_wsgetUriArgValue(Jsi_Interp *interp, struct lws *wsi, Jsi_Value **vPtr, Jsi_Value **oPtr)
{
    int n = 0;
    char buf[JSI_BUFSIZ*8];
    while (lws_hdr_copy_fragment(wsi, buf, sizeof(buf), WSI_TOKEN_HTTP_URI_ARGS, n++) > 0) {
        if (!*vPtr) {
            *vPtr = Jsi_ValueNewArray(interp, NULL, 0);
            Jsi_IncrRefCount(interp, *vPtr);
        }
        Jsi_ValueArraySet(interp, *vPtr, Jsi_ValueNewStringDup(interp, buf), n-1);
        if (!*oPtr) {
            *oPtr = Jsi_ValueNewObj(interp, NULL);
            Jsi_IncrRefCount(interp, *oPtr);
        }
        char *cp = Jsi_Strchr(buf, '=');
        if (cp) {
            *cp = 0;
            Jsi_ValueInsertFixed(interp, *oPtr, buf, Jsi_ValueNewStringDup(interp, cp+1));
        } else {
            Jsi_ValueInsertFixed(interp, *oPtr, buf, Jsi_ValueNewNull(interp));
        }
    }
}