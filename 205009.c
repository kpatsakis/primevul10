void SRP_VBASE_free(SRP_VBASE *vb)
{
    if (!vb)
        return;
    sk_SRP_user_pwd_pop_free(vb->users_pwd, SRP_user_pwd_free);
    sk_SRP_gN_cache_free(vb->gN_cache);
    OPENSSL_free(vb->seed_key);
    OPENSSL_free(vb);
}