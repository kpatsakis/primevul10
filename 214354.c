AP_DECLARE(const char *) ap_auth_name(request_rec *r)
{
    if (authn_ap_auth_name) {
        return authn_ap_auth_name(r);
    }
    return NULL;
}