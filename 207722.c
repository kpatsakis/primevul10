static X509 *ocsp_find_signer_sk(STACK_OF(X509) *certs, OCSP_RESPID *id)
{
    int i, r;
    unsigned char tmphash[SHA_DIGEST_LENGTH], *keyhash;
    EVP_MD *md;
    X509 *x;

    /* Easy if lookup by name */
    if (id->type == V_OCSP_RESPID_NAME)
        return X509_find_by_subject(certs, id->value.byName);

    /* Lookup by key hash */

    /* If key hash isn't SHA1 length then forget it */
    if (id->value.byKey->length != SHA_DIGEST_LENGTH)
        return NULL;
    keyhash = id->value.byKey->data;
    /* Calculate hash of each key and compare */
    for (i = 0; i < sk_X509_num(certs); i++) {
        if ((x = sk_X509_value(certs, i)) != NULL) {
            if ((md = EVP_MD_fetch(x->libctx, SN_sha1, x->propq)) == NULL)
                break;
            r = X509_pubkey_digest(x, md, tmphash, NULL);
            EVP_MD_free(md);
            if (!r)
                break;
            if (memcmp(keyhash, tmphash, SHA_DIGEST_LENGTH) == 0)
                return x;
        }
    }
    return NULL;
}