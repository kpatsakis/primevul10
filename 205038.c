static char *t_tob64(char *dst, const unsigned char *src, int size)
{
    int c, pos = size % 3;
    unsigned char b0 = 0, b1 = 0, b2 = 0, notleading = 0;
    char *olddst = dst;

    switch (pos) {
    case 1:
        b2 = src[0];
        break;
    case 2:
        b1 = src[0];
        b2 = src[1];
        break;
    }

    while (1) {
        c = (b0 & 0xfc) >> 2;
        if (notleading || c != 0) {
            *dst++ = b64table[c];
            notleading = 1;
        }
        c = ((b0 & 3) << 4) | ((b1 & 0xf0) >> 4);
        if (notleading || c != 0) {
            *dst++ = b64table[c];
            notleading = 1;
        }
        c = ((b1 & 0xf) << 2) | ((b2 & 0xc0) >> 6);
        if (notleading || c != 0) {
            *dst++ = b64table[c];
            notleading = 1;
        }
        c = b2 & 0x3f;
        if (notleading || c != 0) {
            *dst++ = b64table[c];
            notleading = 1;
        }
        if (pos >= size)
            break;
        else {
            b0 = src[pos++];
            b1 = src[pos++];
            b2 = src[pos++];
        }
    }

    *dst++ = '\0';
    return olddst;
}