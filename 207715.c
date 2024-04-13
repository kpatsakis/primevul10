int OCSP_resp_get0_signer(OCSP_BASICRESP *bs, X509 **signer,
                          STACK_OF(X509) *extra_certs)
{
    return ocsp_find_signer(signer, bs, extra_certs, 0) > 0;
}