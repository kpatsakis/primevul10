static int ocsp_find_signer(X509 **psigner, OCSP_BASICRESP *bs,
                            STACK_OF(X509) *certs, unsigned long flags)
{
    X509 *signer;
    OCSP_RESPID *rid = &bs->tbsResponseData.responderId;

    if ((signer = ocsp_find_signer_sk(certs, rid)) != NULL) {
        *psigner = signer;
        return 2;
    }
    if ((flags & OCSP_NOINTERN) == 0 &&
        (signer = ocsp_find_signer_sk(bs->certs, rid))) {
        *psigner = signer;
        return 1;
    }
    /* Maybe lookup from store if by subject name */

    *psigner = NULL;
    return 0;
}