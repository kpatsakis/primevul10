encode_LEARN(const struct ofpact_learn *learn,
             enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    const struct ofpact_learn_spec *spec;
    struct nx_action_learn *nal;
    size_t start_ofs;

    start_ofs = out->size;

    if (learn->ofpact.raw == NXAST_RAW_LEARN2
        || learn->limit != 0
        || learn->flags & NX_LEARN_F_WRITE_RESULT) {
        struct nx_action_learn2 *nal2;

        nal2 = put_NXAST_LEARN2(out);
        nal2->limit = htonl(learn->limit);
        nal2->result_dst_ofs = htons(learn->result_dst.ofs);
        nal = &nal2->up;
    } else {
        nal = put_NXAST_LEARN(out);
    }
    nal->idle_timeout = htons(learn->idle_timeout);
    nal->hard_timeout = htons(learn->hard_timeout);
    nal->fin_idle_timeout = htons(learn->fin_idle_timeout);
    nal->fin_hard_timeout = htons(learn->fin_hard_timeout);
    nal->priority = htons(learn->priority);
    nal->cookie = learn->cookie;
    nal->flags = htons(learn->flags);
    nal->table_id = learn->table_id;

    if (learn->flags & NX_LEARN_F_WRITE_RESULT) {
        nx_put_header(out, learn->result_dst.field->id, 0, false);
    }

    OFPACT_LEARN_SPEC_FOR_EACH (spec, learn) {
        put_u16(out, spec->n_bits | spec->dst_type | spec->src_type);

        if (spec->src_type == NX_LEARN_SRC_FIELD) {
            put_u32(out, nxm_header_from_mff(spec->src.field));
            put_u16(out, spec->src.ofs);
        } else {
            size_t n_dst_bytes = 2 * DIV_ROUND_UP(spec->n_bits, 16);
            uint8_t *bits = ofpbuf_put_zeros(out, n_dst_bytes);
            unsigned int n_bytes = DIV_ROUND_UP(spec->n_bits, 8);

            memcpy(bits + n_dst_bytes - n_bytes, ofpact_learn_spec_imm(spec),
                   n_bytes);
        }

        if (spec->dst_type == NX_LEARN_DST_MATCH ||
            spec->dst_type == NX_LEARN_DST_LOAD) {
            put_u32(out, nxm_header_from_mff(spec->dst.field));
            put_u16(out, spec->dst.ofs);
        }
    }

    pad_ofpat(out, start_ofs);
}