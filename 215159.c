static int rle_uncompress(EXRContext *ctx, const uint8_t *src, int compressed_size,
                          int uncompressed_size, EXRThreadData *td)
{
    rle(td->tmp, src, compressed_size, uncompressed_size);

    av_assert1(uncompressed_size % 2 == 0);

    ctx->dsp.predictor(td->tmp, uncompressed_size);
    ctx->dsp.reorder_pixels(td->uncompressed_data, td->tmp, uncompressed_size);

    return 0;
}