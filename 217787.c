encode_WRITE_METADATA(const struct ofpact_metadata *metadata,
                      enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version == OFP10_VERSION) {
        struct nx_action_write_metadata *nawm;

        nawm = put_NXAST_WRITE_METADATA(out);
        nawm->metadata = metadata->metadata;
        nawm->mask = metadata->mask;
    } else {
        struct ofp11_instruction_write_metadata *oiwm;

        oiwm = instruction_put_OFPIT11_WRITE_METADATA(out);
        oiwm->metadata = metadata->metadata;
        oiwm->metadata_mask = metadata->mask;
    }
}