static int huf_build_dec_table(EXRContext *s,
                               EXRThreadData *td, int im, int iM)
{
    int j = 0;

    td->run_sym = -1;
    for (int i = im; i < iM; i++) {
        td->he[j].sym = i;
        td->he[j].len = td->freq[i] & 63;
        td->he[j].code = td->freq[i] >> 6;
        if (td->he[j].len > 32) {
            avpriv_request_sample(s->avctx, "Too big code length");
            return AVERROR_PATCHWELCOME;
        }
        if (td->he[j].len > 0)
            j++;
        else
            td->run_sym = i;
    }

    if (im > 0)
        td->run_sym = 0;
    else if (iM < 65535)
        td->run_sym = 65535;

    if (td->run_sym == -1) {
        avpriv_request_sample(s->avctx, "No place for run symbol");
        return AVERROR_PATCHWELCOME;
    }

    td->he[j].sym = td->run_sym;
    td->he[j].len = td->freq[iM] & 63;
    if (td->he[j].len > 32) {
        avpriv_request_sample(s->avctx, "Too big code length");
        return AVERROR_PATCHWELCOME;
    }
    td->he[j].code = td->freq[iM] >> 6;
    j++;

    ff_free_vlc(&td->vlc);
    return ff_init_vlc_sparse(&td->vlc, 12, j,
                              &td->he[0].len, sizeof(td->he[0]), sizeof(td->he[0].len),
                              &td->he[0].code, sizeof(td->he[0]), sizeof(td->he[0].code),
                              &td->he[0].sym, sizeof(td->he[0]), sizeof(td->he[0].sym), 0);
}