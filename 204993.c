static SRP_user_pwd *SRP_user_pwd_new(void)
{
    SRP_user_pwd *ret = OPENSSL_malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;
    ret->N = NULL;
    ret->g = NULL;
    ret->s = NULL;
    ret->v = NULL;
    ret->id = NULL;
    ret->info = NULL;
    return ret;
}