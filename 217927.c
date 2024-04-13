format_encap_pkt_type(const ovs_be32 pkt_type)
{
    switch (ntohl(pkt_type)) {
    case PT_ETH:
        return "ethernet";
    case PT_NSH:
        return "nsh";
    default:
        return "UNKNOWN";
    }
}