ofpacts_update_instruction_actions(struct ofpbuf *openflow, size_t ofs)
{
    struct ofp11_instruction_actions *oia;

    oia = ofpbuf_at_assert(openflow, ofs, sizeof *oia);
    if (openflow->size > ofs + sizeof *oia) {
        oia->len = htons(openflow->size - ofs);
    } else {
        openflow->size = ofs;
    }
}