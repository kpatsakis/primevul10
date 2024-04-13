AP_DECLARE(const char *) ap_auth_type(request_rec *r)
{
    if (authn_ap_auth_type) {
        return authn_ap_auth_type(r);
    }
    return NULL;
}