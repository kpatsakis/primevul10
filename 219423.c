sync_is_active_scope(const Slapi_DN *dn, Slapi_PBlock *pb)
{
    int rc;
    char *origbase = NULL;
    Slapi_DN *base = NULL;
    int scope;

    slapi_pblock_get(pb, SLAPI_ORIGINAL_TARGET_DN, &origbase);
    slapi_pblock_get(pb, SLAPI_SEARCH_TARGET_SDN, &base);
    slapi_pblock_get(pb, SLAPI_SEARCH_SCOPE, &scope);
    if (NULL == base) {
        base = slapi_sdn_new_dn_byref(origbase);
        slapi_pblock_set(pb, SLAPI_SEARCH_TARGET_SDN, base);
    }
    if (slapi_sdn_scope_test(dn, base, scope)) {
        rc = 1;
    } else {
        rc = 0;
    }

    return (rc);
}