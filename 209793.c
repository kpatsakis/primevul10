void jsi_ToHexStr(const uchar *indata, int dlen, char *out) {
    static char hex[] = "0123456789abcdef";
    int i, n=0;
    for (i=0; i<dlen; i++) {
        int c = indata[i];
        out[n++] = hex[(c>>4)&0xf];
        out[n++] = hex[c&0xf];
    }
    out[n] = 0;
}