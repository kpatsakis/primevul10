pad_ofpat(struct ofpbuf *openflow, size_t start_ofs)
{
    struct ofp_action_header *oah;

    ofpbuf_put_zeros(openflow, PAD_SIZE(openflow->size - start_ofs,
                                        OFP_ACTION_ALIGN));

    oah = ofpbuf_at_assert(openflow, start_ofs, sizeof *oah);
    oah->len = htons(openflow->size - start_ofs);
}