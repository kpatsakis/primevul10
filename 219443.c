sync_build_sort_control(const char *attr)
{
    LDAPControl *ctrl;
    BerElement *ber;
    int rc;

    ber = ber_alloc();
    if (NULL == ber)
        return NULL;

    rc = ber_printf(ber, "{{stb}}", attr, LDAP_TAG_SK_REVERSE, 1);
    if (-1 == rc) {
        ber_free(ber, 1);
        return NULL;
    }

    rc = slapi_build_control(LDAP_CONTROL_SORTREQUEST, ber, 1, &ctrl);

    ber_free(ber, 1);

    if (LDAP_SUCCESS != rc)
        return NULL;

    return ctrl;
}