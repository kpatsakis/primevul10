static int ocsp_verify(OCSP_REQUEST *req, OCSP_BASICRESP *bs,
                       X509 *signer, unsigned long flags)
{
    EVP_PKEY *skey;
    int ret = 1;

    if ((flags & OCSP_NOSIGS) == 0) {
        if ((skey = X509_get0_pubkey(signer)) == NULL) {
            ERR_raise(ERR_LIB_OCSP, OCSP_R_NO_SIGNER_KEY);
            return -1;
        }
        if (req != NULL)
            ret = OCSP_REQUEST_verify(req, skey, signer->libctx, signer->propq);
        else
            ret = OCSP_BASICRESP_verify(bs, skey, signer->libctx, signer->propq);
        if (ret <= 0)
            ERR_raise(ERR_LIB_OCSP, OCSP_R_SIGNATURE_FAILURE);
    }
    return ret;
}