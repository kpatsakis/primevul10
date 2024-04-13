AP_DECLARE(int) ap_satisfies(request_rec *r)
{
    if (access_compat_ap_satisfies) {
        return access_compat_ap_satisfies(r);
    }
    return SATISFY_NOSPEC;
}