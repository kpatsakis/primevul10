static HDNode *zx_getDerivedNode(const char *curve, const uint32_t *address_n,
                                  size_t address_n_count,
                                  uint32_t *fingerprint) {
    static HDNode CONFIDENTIAL node;
    if (fingerprint) {
        *fingerprint = 0;
    }

    if (!get_curve_by_name(curve)) {
       return 0;
    }

    if (!storage_getRootNode(curve, true, &node)) {
        return 0;
    }

    if (!address_n || address_n_count == 0) {
        return &node;
    }

    if (hdnode_private_ckd_cached(&node, address_n, address_n_count,
                                fingerprint) == 0) {
        return 0;
    }

    return &node;
}