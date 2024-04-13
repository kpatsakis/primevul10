SRP_VBASE *SRP_VBASE_new(char *seed_key)
{
    SRP_VBASE *vb = OPENSSL_malloc(sizeof(*vb));

    if (vb == NULL)
        return NULL;
    if ((vb->users_pwd = sk_SRP_user_pwd_new_null()) == NULL
        || (vb->gN_cache = sk_SRP_gN_cache_new_null()) == NULL) {
        OPENSSL_free(vb);
        return NULL;
    }
    vb->default_g = NULL;
    vb->default_N = NULL;
    vb->seed_key = NULL;
    if ((seed_key != NULL) && (vb->seed_key = OPENSSL_strdup(seed_key)) == NULL) {
        sk_SRP_user_pwd_free(vb->users_pwd);
        sk_SRP_gN_cache_free(vb->gN_cache);
        OPENSSL_free(vb);
        return NULL;
    }
    return vb;
}