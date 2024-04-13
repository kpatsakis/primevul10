static void SRP_user_pwd_set_gN(SRP_user_pwd *vinfo, const BIGNUM *g,
                                const BIGNUM *N)
{
    vinfo->N = N;
    vinfo->g = g;
}