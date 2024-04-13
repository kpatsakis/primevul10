learn_min_len(uint16_t header)
{
    int n_bits = header & NX_LEARN_N_BITS_MASK;
    int src_type = header & NX_LEARN_SRC_MASK;
    int dst_type = header & NX_LEARN_DST_MASK;
    unsigned int min_len;

    min_len = 0;
    if (src_type == NX_LEARN_SRC_FIELD) {
        min_len += sizeof(ovs_be32); /* src_field */
        min_len += sizeof(ovs_be16); /* src_ofs */
    } else {
        min_len += 2 * DIV_ROUND_UP(n_bits, 16);
    }
    if (dst_type == NX_LEARN_DST_MATCH ||
        dst_type == NX_LEARN_DST_LOAD) {
        min_len += sizeof(ovs_be32); /* dst_field */
        min_len += sizeof(ovs_be16); /* dst_ofs */
    }
    return min_len;
}