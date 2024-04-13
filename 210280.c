static jsi_JmpPopInfo *jpinfo_new(int off, int topop)
{
    jsi_JmpPopInfo *r = (jsi_JmpPopInfo *)Jsi_Calloc(1, sizeof(*r));
    r->off = off;
    r->topop = topop;
    return r;
}