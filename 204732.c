static int check_hosts(X509 *x, X509_VERIFY_PARAM_ID *id)
{
    int i;
    int n = sk_OPENSSL_STRING_num(id->hosts);
    char *name;

    if (id->peername != NULL) {
        OPENSSL_free(id->peername);
        id->peername = NULL;
    }
    for (i = 0; i < n; ++i) {
        name = sk_OPENSSL_STRING_value(id->hosts, i);
        if (X509_check_host(x, name, 0, id->hostflags, &id->peername) > 0)
            return 1;
    }
    return n == 0;
}