jsi_wsHeader(jsi_wsPss *pss, const char *name, int *lenPtr)
{
    int i, nlen = Jsi_Strlen(name);
    const char *ret = NULL, *cp = Jsi_DSValue(&pss->dHdrs);
    for (i=0; i<pss->hdrNum; i+=2) {
        int sz = pss->hdrSz[i];
        int mat = (!Jsi_Strncasecmp(cp, name, nlen) && cp[nlen]=='=');
        cp += 1 + sz;
        if (mat) {
            ret = cp;
            if (lenPtr)
                *lenPtr = pss->hdrSz[i+1];
            break;
        }
        cp += (1 + pss->hdrSz[i+1]);
    }
    return ret;
}