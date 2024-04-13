create_syncinfo_value(int type, const char *cookie, const char **uuids)
{
    BerElement *ber;
    struct berval *bvp = NULL;

    if ((ber = der_alloc()) == NULL) {
        return (NULL);
    }

    switch (type) {
    case LDAP_TAG_SYNC_NEW_COOKIE:
        ber_printf(ber, "to", type, cookie);
        break;
    case LDAP_TAG_SYNC_REFRESH_DELETE:
    case LDAP_TAG_SYNC_REFRESH_PRESENT:
        ber_printf(ber, "t{", type);
        if (cookie)
            ber_printf(ber, "s", cookie);
        /* ber_printf(ber, "b",1); */
        ber_printf(ber, "}");
        break;
    case LDAP_TAG_SYNC_ID_SET:
        ber_printf(ber, "t{", type);
        if (cookie)
            ber_printf(ber, "s", cookie);
        if (uuids)
            ber_printf(ber, "b[v]", 1, uuids);
        ber_printf(ber, "}");
        break;
    default:
        break;
    }
    ber_flatten(ber, &bvp);
    ber_free(ber, 1);

    return (bvp);
}