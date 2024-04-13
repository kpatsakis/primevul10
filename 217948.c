get_ovsinst_map(enum ofp_version version)
{
    /* OpenFlow 1.1 and 1.2 instructions. */
    static const struct ovsinst_map of11[] = {
        { OVSINST_OFPIT11_GOTO_TABLE, 1 },
        { OVSINST_OFPIT11_WRITE_METADATA, 2 },
        { OVSINST_OFPIT11_WRITE_ACTIONS, 3 },
        { OVSINST_OFPIT11_APPLY_ACTIONS, 4 },
        { OVSINST_OFPIT11_CLEAR_ACTIONS, 5 },
        { 0, -1 },
    };

    /* OpenFlow 1.3+ instructions. */
    static const struct ovsinst_map of13[] = {
        { OVSINST_OFPIT11_GOTO_TABLE, 1 },
        { OVSINST_OFPIT11_WRITE_METADATA, 2 },
        { OVSINST_OFPIT11_WRITE_ACTIONS, 3 },
        { OVSINST_OFPIT11_APPLY_ACTIONS, 4 },
        { OVSINST_OFPIT11_CLEAR_ACTIONS, 5 },
        { OVSINST_OFPIT13_METER, 6 },
        { 0, -1 },
    };

    return version < OFP13_VERSION ? of11 : of13;
}