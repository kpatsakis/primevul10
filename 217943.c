ofpacts_pull_openflow_instructions(struct ofpbuf *openflow,
                                   unsigned int instructions_len,
                                   enum ofp_version version,
                                   const struct vl_mff_map *vl_mff_map,
                                   uint64_t *ofpacts_tlv_bitmap,
                                   struct ofpbuf *ofpacts)
{
    const struct ofp11_instruction *instructions;
    const struct ofp11_instruction *insts[N_OVS_INSTRUCTIONS];
    enum ofperr error;

    ofpbuf_clear(ofpacts);
    if (version == OFP10_VERSION) {
        return ofpacts_pull_openflow_actions__(openflow, instructions_len,
                                               version,
                                               (1u << N_OVS_INSTRUCTIONS) - 1,
                                               ofpacts, 0, vl_mff_map,
                                               ofpacts_tlv_bitmap);
    }

    if (instructions_len % OFP11_INSTRUCTION_ALIGN != 0) {
        VLOG_WARN_RL(&rl, "OpenFlow message instructions length %u is not a "
                     "multiple of %d",
                     instructions_len, OFP11_INSTRUCTION_ALIGN);
        error = OFPERR_OFPBIC_BAD_LEN;
        goto exit;
    }

    instructions = ofpbuf_try_pull(openflow, instructions_len);
    if (instructions == NULL) {
        VLOG_WARN_RL(&rl, "OpenFlow message instructions length %u exceeds "
                     "remaining message length (%"PRIu32")",
                     instructions_len, openflow->size);
        error = OFPERR_OFPBIC_BAD_LEN;
        goto exit;
    }

    error = decode_openflow11_instructions(
        instructions, instructions_len / OFP11_INSTRUCTION_ALIGN,
        insts);
    if (error) {
        goto exit;
    }

    if (insts[OVSINST_OFPIT13_METER]) {
        const struct ofp13_instruction_meter *oim;
        struct ofpact_meter *om;

        oim = ALIGNED_CAST(const struct ofp13_instruction_meter *,
                           insts[OVSINST_OFPIT13_METER]);

        om = ofpact_put_METER(ofpacts);
        om->meter_id = ntohl(oim->meter_id);
        om->provider_meter_id = UINT32_MAX; /* No provider meter ID. */
    }
    if (insts[OVSINST_OFPIT11_APPLY_ACTIONS]) {
        const struct ofp_action_header *actions;
        size_t actions_len;

        get_actions_from_instruction(insts[OVSINST_OFPIT11_APPLY_ACTIONS],
                                     &actions, &actions_len);
        error = ofpacts_decode(actions, actions_len, version, vl_mff_map,
                               ofpacts_tlv_bitmap, ofpacts);
        if (error) {
            goto exit;
        }
    }
    if (insts[OVSINST_OFPIT11_CLEAR_ACTIONS]) {
        instruction_get_OFPIT11_CLEAR_ACTIONS(
            insts[OVSINST_OFPIT11_CLEAR_ACTIONS]);
        ofpact_put_CLEAR_ACTIONS(ofpacts);
    }
    if (insts[OVSINST_OFPIT11_WRITE_ACTIONS]) {
        struct ofpact_nest *on;
        const struct ofp_action_header *actions;
        size_t actions_len;
        size_t start = ofpacts->size;
        ofpact_put(ofpacts, OFPACT_WRITE_ACTIONS,
                   offsetof(struct ofpact_nest, actions));
        get_actions_from_instruction(insts[OVSINST_OFPIT11_WRITE_ACTIONS],
                                     &actions, &actions_len);
        error = ofpacts_decode_for_action_set(actions, actions_len,
                                              version, vl_mff_map,
                                              ofpacts_tlv_bitmap, ofpacts);
        if (error) {
            goto exit;
        }
        on = ofpbuf_at_assert(ofpacts, start, sizeof *on);
        on->ofpact.len = ofpacts->size - start;
    }
    if (insts[OVSINST_OFPIT11_WRITE_METADATA]) {
        const struct ofp11_instruction_write_metadata *oiwm;
        struct ofpact_metadata *om;

        oiwm = ALIGNED_CAST(const struct ofp11_instruction_write_metadata *,
                            insts[OVSINST_OFPIT11_WRITE_METADATA]);

        om = ofpact_put_WRITE_METADATA(ofpacts);
        om->metadata = oiwm->metadata;
        om->mask = oiwm->metadata_mask;
    }
    if (insts[OVSINST_OFPIT11_GOTO_TABLE]) {
        const struct ofp11_instruction_goto_table *oigt;
        struct ofpact_goto_table *ogt;

        oigt = instruction_get_OFPIT11_GOTO_TABLE(
            insts[OVSINST_OFPIT11_GOTO_TABLE]);
        ogt = ofpact_put_GOTO_TABLE(ofpacts);
        ogt->table_id = oigt->table_id;
    }

    error = ofpacts_verify(ofpacts->data, ofpacts->size,
                           (1u << N_OVS_INSTRUCTIONS) - 1, 0);
exit:
    if (error) {
        ofpbuf_clear(ofpacts);
    }
    return error;
}