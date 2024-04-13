cdata_free (cdata_t *cd)
{
    z_stream *z = &cd->z;

    if (cd->decomp.comptype == GCAB_COMPRESSION_LZX) {
        LZXfdi_clear (&cd->decomp);
    }

    if (cd->decomp.comptype == GCAB_COMPRESSION_MSZIP) {
        if (z->opaque) {
            inflateEnd (z);
            z->opaque = NULL;
        }
    }
    g_free (cd->reserved);
    g_free (cd);
}