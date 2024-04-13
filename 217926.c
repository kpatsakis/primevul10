decode_NXAST_RAW_NOTE(const struct nx_action_note *nan,
                      enum ofp_version ofp_version OVS_UNUSED,
                      struct ofpbuf *out)
{
    struct ofpact_note *note;
    unsigned int length;

    length = ntohs(nan->len) - offsetof(struct nx_action_note, note);
    note = ofpact_put_NOTE(out);
    note->length = length;
    ofpbuf_put(out, nan->note, length);
    note = out->header;
    ofpact_finish_NOTE(out, &note);

    return 0;
}