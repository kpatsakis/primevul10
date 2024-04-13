static int dsa_pub_decode(EVP_PKEY *pkey, X509_PUBKEY *pubkey)
{
    const unsigned char *p, *pm;
    int pklen, pmlen;
    int ptype;
    void *pval;
    ASN1_STRING *pstr;
    X509_ALGOR *palg;
    ASN1_INTEGER *public_key = NULL;

    DSA *dsa = NULL;

    if (!X509_PUBKEY_get0_param(NULL, &p, &pklen, &palg, pubkey))
        return 0;
    X509_ALGOR_get0(NULL, &ptype, &pval, palg);

    if (ptype == V_ASN1_SEQUENCE) {
        pstr = pval;
        pm = pstr->data;
        pmlen = pstr->length;

        if ((dsa = d2i_DSAparams(NULL, &pm, pmlen)) == NULL) {
            DSAerr(DSA_F_DSA_PUB_DECODE, DSA_R_DECODE_ERROR);
            goto err;
        }

    } else if ((ptype == V_ASN1_NULL) || (ptype == V_ASN1_UNDEF)) {
        if ((dsa = DSA_new()) == NULL) {
            DSAerr(DSA_F_DSA_PUB_DECODE, ERR_R_MALLOC_FAILURE);
            goto err;
        }
    } else {
        DSAerr(DSA_F_DSA_PUB_DECODE, DSA_R_PARAMETER_ENCODING_ERROR);
        goto err;
    }

    if ((public_key = d2i_ASN1_INTEGER(NULL, &p, pklen)) == NULL) {
        DSAerr(DSA_F_DSA_PUB_DECODE, DSA_R_DECODE_ERROR);
        goto err;
    }

    if ((dsa->pub_key = ASN1_INTEGER_to_BN(public_key, NULL)) == NULL) {
        DSAerr(DSA_F_DSA_PUB_DECODE, DSA_R_BN_DECODE_ERROR);
        goto err;
    }

    ASN1_INTEGER_free(public_key);
    EVP_PKEY_assign_DSA(pkey, dsa);
    return 1;

 err:
    ASN1_INTEGER_free(public_key);
    DSA_free(dsa);
    return 0;

}