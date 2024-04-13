int OCSP_request_verify(OCSP_REQUEST *req, STACK_OF(X509) *certs,
                        X509_STORE *store, unsigned long flags)
{
    X509 *signer;
    const X509_NAME *nm;
    GENERAL_NAME *gen;
    int ret;

    if (!req->optionalSignature) {
        ERR_raise(ERR_LIB_OCSP, OCSP_R_REQUEST_NOT_SIGNED);
        return 0;
    }
    gen = req->tbsRequest.requestorName;
    if (!gen || gen->type != GEN_DIRNAME) {
        ERR_raise(ERR_LIB_OCSP, OCSP_R_UNSUPPORTED_REQUESTORNAME_TYPE);
        return 0; /* not returning -1 here for backward compatibility*/
    }
    nm = gen->d.directoryName;
    ret = ocsp_req_find_signer(&signer, req, nm, certs, flags);
    if (ret <= 0) {
        ERR_raise(ERR_LIB_OCSP, OCSP_R_SIGNER_CERTIFICATE_NOT_FOUND);
        return 0; /* not returning -1 here for backward compatibility*/
    }
    if ((ret == 2) && (flags & OCSP_TRUSTOTHER) != 0)
        flags |= OCSP_NOVERIFY;

    if ((ret = ocsp_verify(req, NULL, signer, flags)) <= 0)
        return 0; /* not returning 'ret' here for backward compatibility*/
    if ((flags & OCSP_NOVERIFY) != 0)
        return 1;
    return ocsp_verify_signer(signer, 0, store, flags,
                              (flags & OCSP_NOCHAIN) != 0 ?
                              NULL : req->optionalSignature->certs, NULL) > 0;
    /* using '> 0' here to avoid breaking backward compatibility returning -1 */
}