static int SRP_user_pwd_set_sv_BN(SRP_user_pwd *vinfo, BIGNUM *s, BIGNUM *v)
{
    vinfo->v = v;
    vinfo->s = s;
    return (vinfo->s != NULL && vinfo->v != NULL);
}