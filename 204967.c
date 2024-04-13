static int dsa_pub_encode(X509_PUBKEY *pk, const EVP_PKEY *pkey)
{
    DSA *dsa;
    int ptype;
    unsigned char *penc = NULL;
    int penclen;
    ASN1_STRING *str = NULL;
    ASN1_INTEGER *pubint = NULL;

    dsa = pkey->pkey.dsa;
    if (pkey->save_parameters && dsa->p && dsa->q && dsa->g) {
        str = ASN1_STRING_new();
        if (str == NULL) {
            DSAerr(DSA_F_DSA_PUB_ENCODE, ERR_R_MALLOC_FAILURE);
            goto err;
        }
        str->length = i2d_DSAparams(dsa, &str->data);
        if (str->length <= 0) {
            DSAerr(DSA_F_DSA_PUB_ENCODE, ERR_R_MALLOC_FAILURE);
            goto err;
        }
        ptype = V_ASN1_SEQUENCE;
    } else
        ptype = V_ASN1_UNDEF;

    pubint = BN_to_ASN1_INTEGER(dsa->pub_key, NULL);

    if (pubint == NULL) {
        DSAerr(DSA_F_DSA_PUB_ENCODE, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    penclen = i2d_ASN1_INTEGER(pubint, &penc);
    ASN1_INTEGER_free(pubint);

    if (penclen <= 0) {
        DSAerr(DSA_F_DSA_PUB_ENCODE, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    if (X509_PUBKEY_set0_param(pk, OBJ_nid2obj(EVP_PKEY_DSA),
                               ptype, str, penc, penclen))
        return 1;

 err:
    OPENSSL_free(penc);
    ASN1_STRING_free(str);

    return 0;
}