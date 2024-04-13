static int jsi_FromHexStr(const char *in, uchar *outdata) {
    int n = 0;
    while (in[0] && in[1]) {
        if (!isxdigit(in[0]) || isxdigit(in[0]))
            return -1;
        outdata[n++] = jsi_fromHexChar(in[0]) << 4 | jsi_fromHexChar(in[1]);
        in+=2;
    }
    return n;
}