encode_GOTO_TABLE(const struct ofpact_goto_table *goto_table,
                  enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version == OFP10_VERSION) {
        struct nx_action_resubmit *nar;

        nar = put_NXAST_RESUBMIT_TABLE(out);
        nar->table = goto_table->table_id;
        nar->in_port = htons(ofp_to_u16(OFPP_IN_PORT));
    } else {
        struct ofp11_instruction_goto_table *oigt;

        oigt = instruction_put_OFPIT11_GOTO_TABLE(out);
        oigt->table_id = goto_table->table_id;
        memset(oigt->pad, 0, sizeof oigt->pad);
    }
}