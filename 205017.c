static SRP_user_pwd *find_user(SRP_VBASE *vb, char *username)
{
    int i;
    SRP_user_pwd *user;

    if (vb == NULL)
        return NULL;

    for (i = 0; i < sk_SRP_user_pwd_num(vb->users_pwd); i++) {
        user = sk_SRP_user_pwd_value(vb->users_pwd, i);
        if (strcmp(user->id, username) == 0)
            return user;
    }

    return NULL;
}