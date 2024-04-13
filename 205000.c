static SRP_user_pwd *srp_user_pwd_dup(SRP_user_pwd *src)
{
    SRP_user_pwd *ret;

    if (src == NULL)
        return NULL;
    if ((ret = SRP_user_pwd_new()) == NULL)
        return NULL;

    SRP_user_pwd_set_gN(ret, src->g, src->N);
    if (!SRP_user_pwd_set_ids(ret, src->id, src->info)
        || !SRP_user_pwd_set_sv_BN(ret, BN_dup(src->s), BN_dup(src->v))) {
            SRP_user_pwd_free(ret);
            return NULL;
    }
    return ret;
}