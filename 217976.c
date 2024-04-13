parse_encap_header(const char *hdr, ovs_be32 *packet_type)
{
    if (strcmp(hdr, "ethernet") == 0) {
        *packet_type = htonl(PT_ETH);
    } else if (strcmp(hdr, "nsh") == 0) {
        *packet_type = htonl(PT_NSH);
    } else {
        return false;
    }
    return true;
}