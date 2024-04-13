static int SRP_user_pwd_set_ids(SRP_user_pwd *vinfo, const char *id,
                                const char *info)
{
    if (id != NULL && NULL == (vinfo->id = OPENSSL_strdup(id)))
        return 0;
    return (info == NULL || NULL != (vinfo->info = OPENSSL_strdup(info)));
}