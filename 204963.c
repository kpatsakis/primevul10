static int dsa_pub_print(BIO *bp, const EVP_PKEY *pkey, int indent,
                         ASN1_PCTX *ctx)
{
    return do_dsa_print(bp, pkey->pkey.dsa, indent, 1);
}