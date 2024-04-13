static int ocsp_check_delegated(X509 *x)
{
    if ((X509_get_extension_flags(x) & EXFLAG_XKUSAGE)
        && (X509_get_extended_key_usage(x) & XKU_OCSP_SIGN))
        return 1;
    ERR_raise(ERR_LIB_OCSP, OCSP_R_MISSING_OCSPSIGNING_USAGE);
    return 0;
}