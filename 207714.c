static int ocsp_match_issuerid(X509 *cert, OCSP_CERTID *cid,
                               STACK_OF(OCSP_SINGLERESP) *sresp)
{
    int ret = -1;
    EVP_MD *dgst = NULL;

    /* If only one ID to match then do it */
    if (cid != NULL) {
        char name[OSSL_MAX_NAME_SIZE];
        const X509_NAME *iname;
        int mdlen;
        unsigned char md[EVP_MAX_MD_SIZE];

        OBJ_obj2txt(name, sizeof(name), cid->hashAlgorithm.algorithm, 0);

        (void)ERR_set_mark();
        dgst = EVP_MD_fetch(NULL, name, NULL);
        if (dgst == NULL)
            dgst = (EVP_MD *)EVP_get_digestbyname(name);

        if (dgst == NULL) {
            (void)ERR_clear_last_mark();
            ERR_raise(ERR_LIB_OCSP, OCSP_R_UNKNOWN_MESSAGE_DIGEST);
            goto end;
        }
        (void)ERR_pop_to_mark();

        mdlen = EVP_MD_get_size(dgst);
        if (mdlen < 0) {
            ERR_raise(ERR_LIB_OCSP, OCSP_R_DIGEST_SIZE_ERR);
            goto end;
        }
        if (cid->issuerNameHash.length != mdlen ||
            cid->issuerKeyHash.length != mdlen) {
            ret = 0;
            goto end;
        }
        iname = X509_get_subject_name(cert);
        if (!X509_NAME_digest(iname, dgst, md, NULL))
            goto end;
        if (memcmp(md, cid->issuerNameHash.data, mdlen) != 0) {
            ret = 0;
            goto end;
        }
        if (!X509_pubkey_digest(cert, dgst, md, NULL)) {
            ERR_raise(ERR_LIB_OCSP, OCSP_R_DIGEST_ERR);
            goto end;
        }
        ret = memcmp(md, cid->issuerKeyHash.data, mdlen) == 0;
        goto end;
    } else {
        /* We have to match the whole lot */
        int i;
        OCSP_CERTID *tmpid;

        for (i = 0; i < sk_OCSP_SINGLERESP_num(sresp); i++) {
            tmpid = sk_OCSP_SINGLERESP_value(sresp, i)->certId;
            ret = ocsp_match_issuerid(cert, tmpid, NULL);
            if (ret <= 0)
                return ret;
        }
    }
    return 1;
end:
    EVP_MD_free(dgst);
    return ret;
}