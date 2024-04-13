static int trust_compat(X509_TRUST *trust, X509 *x, int flags)
{
    X509_check_purpose(x, -1, 0);
    if (x->ex_flags & EXFLAG_SS)
        return X509_TRUST_TRUSTED;
    else
        return X509_TRUST_UNTRUSTED;
}