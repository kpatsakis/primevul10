static int ocsp_req_find_signer(X509 **psigner, OCSP_REQUEST *req,
                                const X509_NAME *nm, STACK_OF(X509) *certs,
                                unsigned long flags)
{
    X509 *signer;

    if ((flags & OCSP_NOINTERN) == 0) {
        signer = X509_find_by_subject(req->optionalSignature->certs, nm);
        if (signer != NULL) {
            *psigner = signer;
            return 1;
        }
    }

    if ((signer = X509_find_by_subject(certs, nm)) != NULL) {
        *psigner = signer;
        return 2;
    }
    return 0;
}