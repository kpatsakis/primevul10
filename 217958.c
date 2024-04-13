ofpacts_put_openflow_instructions(const struct ofpact ofpacts[],
                                  size_t ofpacts_len,
                                  struct ofpbuf *openflow,
                                  enum ofp_version ofp_version)
{
    const struct ofpact *end = ofpact_end(ofpacts, ofpacts_len);
    const struct ofpact *a;

    if (ofp_version == OFP10_VERSION) {
        ofpacts_put_openflow_actions(ofpacts, ofpacts_len, openflow,
                                     ofp_version);
        return;
    }

    a = ofpacts;
    while (a < end) {
        if (ofpact_is_apply_actions(a)) {
            size_t ofs = openflow->size;

            instruction_put_OFPIT11_APPLY_ACTIONS(openflow);
            do {
                encode_ofpact(a, ofp_version, openflow);
                a = ofpact_next(a);
            } while (a < end && ofpact_is_apply_actions(a));
            ofpacts_update_instruction_actions(openflow, ofs);
        } else {
            encode_ofpact(a, ofp_version, openflow);
            a = ofpact_next(a);
        }
    }
}