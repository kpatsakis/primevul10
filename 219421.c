sync_parse_control_value(struct berval *psbvp, ber_int_t *mode, int *reload, char **cookie)
{
    int rc = LDAP_SUCCESS;

    if (psbvp->bv_len == 0 || psbvp->bv_val == NULL) {
        rc = LDAP_PROTOCOL_ERROR;
    } else {
        BerElement *ber = ber_init(psbvp);
        if (ber == NULL) {
            rc = LDAP_OPERATIONS_ERROR;
        } else {
            if (ber_scanf(ber, "{e", mode) == LBER_ERROR) {
                rc = LDAP_PROTOCOL_ERROR;
            } else {
                ber_tag_t tag;
                ber_len_t len;
                tag = ber_peek_tag(ber, &len);
                if (tag == LDAP_TAG_SYNC_COOKIE) {
                    rc = ber_scanf(ber, "a", cookie);
                    tag = ber_peek_tag(ber, &len);
                }
                if (rc != LBER_ERROR && tag == LDAP_TAG_RELOAD_HINT) {
                    rc = ber_scanf(ber, "b", reload);
                }
                if (rc != LBER_ERROR) {
                    rc = ber_scanf(ber, "}");
                }
                if (rc == LBER_ERROR) {

                    rc = LDAP_PROTOCOL_ERROR;
                };
            }

            /* the ber encoding is no longer needed */
            ber_free(ber, 1);
        }
    }

    return (rc);
}