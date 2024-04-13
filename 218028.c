ofpacts_pull(struct ofpbuf *ofpacts)
{
    size_t ofs;

    ofs = ofpacts->size;
    ofpbuf_pull(ofpacts, ofs);

    return ofs;
}