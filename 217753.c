decode_NXAST_RAW_LEARN2(const struct nx_action_learn2 *nal,
                        enum ofp_version ofp_version OVS_UNUSED,
                        const struct vl_mff_map *vl_mff_map,
                        uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpbuf b = ofpbuf_const_initializer(nal, ntohs(nal->up.len));
    struct ofpact_learn *learn;
    enum ofperr error;

    if (nal->pad2) {
        return OFPERR_NXBAC_MUST_BE_ZERO;
    }

    learn = ofpact_put_LEARN(ofpacts);
    error = decode_LEARN_common(&nal->up, NXAST_RAW_LEARN2, learn);
    if (error) {
        return error;
    }

    learn->limit = ntohl(nal->limit);

    if (learn->flags & ~(NX_LEARN_F_SEND_FLOW_REM |
                         NX_LEARN_F_DELETE_LEARNED |
                         NX_LEARN_F_WRITE_RESULT)) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    ofpbuf_pull(&b, sizeof *nal);

    if (learn->flags & NX_LEARN_F_WRITE_RESULT) {
        error = nx_pull_header(&b, vl_mff_map, &learn->result_dst.field, NULL);
        if (error) {
            return error;
        }
        if (!learn->result_dst.field->writable) {
            return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
        }
        learn->result_dst.ofs = ntohs(nal->result_dst_ofs);
        learn->result_dst.n_bits = 1;
    } else if (nal->result_dst_ofs) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    return decode_LEARN_specs(b.data, (char *) nal + ntohs(nal->up.len),
                              vl_mff_map, tlv_bitmap, ofpacts);
}