static int dsa_priv_encode(PKCS8_PRIV_KEY_INFO *p8, const EVP_PKEY *pkey)
{
    ASN1_STRING *params = NULL;
    ASN1_INTEGER *prkey = NULL;
    unsigned char *dp = NULL;
    int dplen;

    if (!pkey->pkey.dsa || !pkey->pkey.dsa->priv_key) {
        DSAerr(DSA_F_DSA_PRIV_ENCODE, DSA_R_MISSING_PARAMETERS);
        goto err;
    }

    params = ASN1_STRING_new();

    if (params == NULL) {
        DSAerr(DSA_F_DSA_PRIV_ENCODE, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    params->length = i2d_DSAparams(pkey->pkey.dsa, &params->data);
    if (params->length <= 0) {
        DSAerr(DSA_F_DSA_PRIV_ENCODE, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    params->type = V_ASN1_SEQUENCE;

    /* Get private key into integer */
    prkey = BN_to_ASN1_INTEGER(pkey->pkey.dsa->priv_key, NULL);

    if (!prkey) {
        DSAerr(DSA_F_DSA_PRIV_ENCODE, DSA_R_BN_ERROR);
        goto err;
    }

    dplen = i2d_ASN1_INTEGER(prkey, &dp);

    ASN1_STRING_clear_free(prkey);
    prkey = NULL;

    if (!PKCS8_pkey_set0(p8, OBJ_nid2obj(NID_dsa), 0,
                         V_ASN1_SEQUENCE, params, dp, dplen))
        goto err;

    return 1;

 err:
    OPENSSL_free(dp);
    ASN1_STRING_free(params);
    ASN1_STRING_clear_free(prkey);
    return 0;
}