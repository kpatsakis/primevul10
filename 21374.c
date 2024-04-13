static void tight_pack24(VncState *vs, uint8_t *buf, size_t count, size_t *ret)
{
    uint8_t *buf8;
    uint32_t pix;
    int rshift, gshift, bshift;

    buf8 = buf;

    if (1 /* FIXME */) {
        rshift = vs->client_pf.rshift;
        gshift = vs->client_pf.gshift;
        bshift = vs->client_pf.bshift;
    } else {
        rshift = 24 - vs->client_pf.rshift;
        gshift = 24 - vs->client_pf.gshift;
        bshift = 24 - vs->client_pf.bshift;
    }

    if (ret) {
        *ret = count * 3;
    }

    while (count--) {
        pix = ldl_he_p(buf8);
        *buf++ = (char)(pix >> rshift);
        *buf++ = (char)(pix >> gshift);
        *buf++ = (char)(pix >> bshift);
        buf8 += 4;
    }
}