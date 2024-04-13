static int huf_uncompress(EXRContext *s,
                          EXRThreadData *td,
                          GetByteContext *gb,
                          uint16_t *dst, int dst_size)
{
    int32_t im, iM;
    uint32_t nBits;
    int ret;

    im       = bytestream2_get_le32(gb);
    iM       = bytestream2_get_le32(gb);
    bytestream2_skip(gb, 4);
    nBits = bytestream2_get_le32(gb);
    if (im < 0 || im >= HUF_ENCSIZE ||
        iM < 0 || iM >= HUF_ENCSIZE)
        return AVERROR_INVALIDDATA;

    bytestream2_skip(gb, 4);

    if (!td->freq)
        td->freq = av_malloc_array(HUF_ENCSIZE, sizeof(*td->freq));
    if (!td->he)
        td->he = av_calloc(HUF_ENCSIZE, sizeof(*td->he));
    if (!td->freq || !td->he) {
        ret = AVERROR(ENOMEM);
        return ret;
    }

    memset(td->freq, 0, sizeof(*td->freq) * HUF_ENCSIZE);
    if ((ret = huf_unpack_enc_table(gb, im, iM, td->freq)) < 0)
        return ret;

    if (nBits > 8 * bytestream2_get_bytes_left(gb)) {
        ret = AVERROR_INVALIDDATA;
        return ret;
    }

    if ((ret = huf_build_dec_table(s, td, im, iM)) < 0)
        return ret;
    return huf_decode(&td->vlc, gb, nBits, td->run_sym, dst_size, dst);
}