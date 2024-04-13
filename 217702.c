encode_WRITE_ACTIONS(const struct ofpact_nest *actions,
                     enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version > OFP10_VERSION) {
        const size_t ofs = out->size;

        instruction_put_OFPIT11_WRITE_ACTIONS(out);
        ofpacts_put_openflow_actions(actions->actions,
                                     ofpact_nest_get_action_len(actions),
                                     out, ofp_version);
        ofpacts_update_instruction_actions(out, ofs);
    }
}