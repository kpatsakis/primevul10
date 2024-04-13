encode_NOTE(const struct ofpact_note *note,
            enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    size_t start_ofs = out->size;
    struct nx_action_note *nan;

    put_NXAST_NOTE(out);
    out->size = out->size - sizeof nan->note;

    ofpbuf_put(out, note->data, note->length);
    pad_ofpat(out, start_ofs);
}