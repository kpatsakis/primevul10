sync_create_sync_done_control(LDAPControl **ctrlp, int refresh, char *cookie)
{
    int rc;
    BerElement *ber;
    struct berval *bvp;

    if (ctrlp == NULL || (ber = der_alloc()) == NULL) {
        return (LDAP_OPERATIONS_ERROR);
    }

    *ctrlp = NULL;

    if (cookie) {
        if ((rc = ber_printf(ber, "{s", cookie)) != -1) {
            if (refresh) {
                rc = ber_printf(ber, "e}", refresh);
            } else {
                rc = ber_printf(ber, "}");
            }
        }
    } else {
        if (refresh) {
            rc = ber_printf(ber, "{e}", refresh);
        } else {
            rc = ber_printf(ber, "{}");
        }
    }
    if (rc != -1) {
        rc = ber_flatten(ber, &bvp);
    }
    ber_free(ber, 1);

    if (rc == -1) {
        return (LDAP_OPERATIONS_ERROR);
    }

    *ctrlp = (LDAPControl *)slapi_ch_malloc(sizeof(LDAPControl));
    (*ctrlp)->ldctl_iscritical = 0;
    (*ctrlp)->ldctl_oid = slapi_ch_strdup(LDAP_CONTROL_SYNC_DONE);
    (*ctrlp)->ldctl_value = *bvp; /* struct copy */

    bvp->bv_val = NULL;
    ber_bvfree(bvp);

    return (LDAP_SUCCESS);
}