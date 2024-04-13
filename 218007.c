set_field_to_legacy_openflow(const struct ofpact_set_field *sf,
                             enum ofp_version ofp_version,
                             struct ofpbuf *out)
{
    switch ((int) sf->field->id) {
    case MFF_VLAN_TCI: {
        ovs_be16 tci = sf->value->be16;
        bool cfi = (tci & htons(VLAN_CFI)) != 0;
        uint16_t vid = vlan_tci_to_vid(tci);
        uint8_t pcp = vlan_tci_to_pcp(tci);

        if (ofp_version < OFP11_VERSION) {
            /* NXM_OF_VLAN_TCI to OpenFlow 1.0 mapping:
             *
             * If CFI=1, Add or modify VLAN VID & PCP.
             * If CFI=0, strip VLAN header, if any.
             */
            if (cfi) {
                put_OFPAT10_SET_VLAN_VID(out, vid);
                put_OFPAT10_SET_VLAN_PCP(out, pcp);
            } else {
                put_OFPAT10_STRIP_VLAN(out);
            }
        } else {
            /* NXM_OF_VLAN_TCI to OpenFlow 1.1 mapping:
             *
             * If CFI=1, Add or modify VLAN VID & PCP.
             *    OpenFlow 1.1 set actions only apply if the packet
             *    already has VLAN tags.  To be sure that is the case
             *    we have to push a VLAN header.  As we do not support
             *    multiple layers of VLANs, this is a no-op, if a VLAN
             *    header already exists.  This may backfire, however,
             *    when we start supporting multiple layers of VLANs.
             * If CFI=0, strip VLAN header, if any.
             */
            if (cfi) {
                /* Push a VLAN tag, if one was not seen at action validation
                 * time. */
                if (!sf->flow_has_vlan) {
                    put_OFPAT11_PUSH_VLAN(out, htons(ETH_TYPE_VLAN_8021Q));
                }
                put_OFPAT11_SET_VLAN_VID(out, vid);
                put_OFPAT11_SET_VLAN_PCP(out, pcp);
            } else {
                /* If the flow did not match on vlan, we have no way of
                 * knowing if the vlan tag exists, so we must POP just to be
                 * sure. */
                put_OFPAT11_POP_VLAN(out);
            }
        }
        break;
    }

    case MFF_VLAN_VID: {
        uint16_t vid = ntohs(sf->value->be16) & VLAN_VID_MASK;
        if (ofp_version == OFP10_VERSION) {
            put_OFPAT10_SET_VLAN_VID(out, vid);
        } else {
            put_OFPAT11_SET_VLAN_VID(out, vid);
        }
        break;
    }

    case MFF_VLAN_PCP:
        if (ofp_version == OFP10_VERSION) {
            put_OFPAT10_SET_VLAN_PCP(out, sf->value->u8);
        } else {
            put_OFPAT11_SET_VLAN_PCP(out, sf->value->u8);
        }
        break;

    case MFF_ETH_SRC:
        put_OFPAT_SET_DL_SRC(out, ofp_version)->dl_addr = sf->value->mac;
        break;

    case MFF_ETH_DST:
        put_OFPAT_SET_DL_DST(out, ofp_version)->dl_addr = sf->value->mac;
        break;

    case MFF_IPV4_SRC:
        put_OFPAT_SET_NW_SRC(out, ofp_version, sf->value->be32);
        break;

    case MFF_IPV4_DST:
        put_OFPAT_SET_NW_DST(out, ofp_version, sf->value->be32);
        break;

    case MFF_IP_DSCP:
        put_OFPAT_SET_NW_TOS(out, ofp_version, sf->value->u8);
        break;

    case MFF_IP_DSCP_SHIFTED:
        put_OFPAT_SET_NW_TOS(out, ofp_version, sf->value->u8 << 2);
        break;

    case MFF_IP_ECN: {
        struct ofpact_ecn ip_ecn = { .ecn = sf->value->u8 };
        encode_SET_IP_ECN(&ip_ecn, ofp_version, out);
        break;
    }

    case MFF_TCP_SRC:
    case MFF_UDP_SRC:
        put_OFPAT_SET_TP_SRC(out, sf->value->be16);
        break;

    case MFF_TCP_DST:
    case MFF_UDP_DST:
        put_OFPAT_SET_TP_DST(out, sf->value->be16);
        break;

    default:
        set_field_to_nxast(sf, out);
        break;
    }
}