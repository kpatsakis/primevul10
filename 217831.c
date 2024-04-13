encode_SET_L4_SRC_PORT(const struct ofpact_l4_port *l4_port,
                       enum ofp_version ofp_version, struct ofpbuf *out)
{
    uint8_t proto = l4_port->flow_ip_proto;
    enum mf_field_id field = (proto == IPPROTO_TCP ? MFF_TCP_SRC
                              : proto == IPPROTO_UDP ? MFF_UDP_SRC
                              : proto == IPPROTO_SCTP ? MFF_SCTP_SRC
                              : MFF_N_IDS);

    encode_SET_L4_port(l4_port, ofp_version, OFPAT_RAW_SET_TP_SRC, field, out);
}