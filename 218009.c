decode_set_vlan_pcp(uint8_t pcp, bool push_vlan_if_needed, struct ofpbuf *out)
{
    if (pcp & ~7) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    } else {
        struct ofpact_vlan_pcp *vlan_pcp = ofpact_put_SET_VLAN_PCP(out);
        vlan_pcp->vlan_pcp = pcp;
        vlan_pcp->push_vlan_if_needed = push_vlan_if_needed;
        return 0;
    }
}