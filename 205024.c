int SRP_VBASE_init(SRP_VBASE *vb, char *verifier_file)
{
    int error_code;
    STACK_OF(SRP_gN) *SRP_gN_tab = sk_SRP_gN_new_null();
    char *last_index = NULL;
    int i;
    char **pp;

    SRP_gN *gN = NULL;
    SRP_user_pwd *user_pwd = NULL;

    TXT_DB *tmpdb = NULL;
    BIO *in = BIO_new(BIO_s_file());

    error_code = SRP_ERR_OPEN_FILE;

    if (in == NULL || BIO_read_filename(in, verifier_file) <= 0)
        goto err;

    error_code = SRP_ERR_VBASE_INCOMPLETE_FILE;

    if ((tmpdb = TXT_DB_read(in, DB_NUMBER)) == NULL)
        goto err;

    error_code = SRP_ERR_MEMORY;

    if (vb->seed_key) {
        last_index = SRP_get_default_gN(NULL)->id;
    }
    for (i = 0; i < sk_OPENSSL_PSTRING_num(tmpdb->data); i++) {
        pp = sk_OPENSSL_PSTRING_value(tmpdb->data, i);
        if (pp[DB_srptype][0] == DB_SRP_INDEX) {
            /*
             * we add this couple in the internal Stack
             */

            if ((gN = OPENSSL_malloc(sizeof(*gN))) == NULL)
                goto err;

            if ((gN->id = OPENSSL_strdup(pp[DB_srpid])) == NULL
                || (gN->N = SRP_gN_place_bn(vb->gN_cache, pp[DB_srpverifier]))
                        == NULL
                || (gN->g = SRP_gN_place_bn(vb->gN_cache, pp[DB_srpsalt]))
                        == NULL
                || sk_SRP_gN_insert(SRP_gN_tab, gN, 0) == 0)
                goto err;

            gN = NULL;

            if (vb->seed_key != NULL) {
                last_index = pp[DB_srpid];
            }
        } else if (pp[DB_srptype][0] == DB_SRP_VALID) {
            /* it is a user .... */
            const SRP_gN *lgN;

            if ((lgN = SRP_get_gN_by_id(pp[DB_srpgN], SRP_gN_tab)) != NULL) {
                error_code = SRP_ERR_MEMORY;
                if ((user_pwd = SRP_user_pwd_new()) == NULL)
                    goto err;

                SRP_user_pwd_set_gN(user_pwd, lgN->g, lgN->N);
                if (!SRP_user_pwd_set_ids
                    (user_pwd, pp[DB_srpid], pp[DB_srpinfo]))
                    goto err;

                error_code = SRP_ERR_VBASE_BN_LIB;
                if (!SRP_user_pwd_set_sv
                    (user_pwd, pp[DB_srpsalt], pp[DB_srpverifier]))
                    goto err;

                if (sk_SRP_user_pwd_insert(vb->users_pwd, user_pwd, 0) == 0)
                    goto err;
                user_pwd = NULL; /* abandon responsability */
            }
        }
    }

    if (last_index != NULL) {
        /* this means that we want to simulate a default user */

        if (((gN = SRP_get_gN_by_id(last_index, SRP_gN_tab)) == NULL)) {
            error_code = SRP_ERR_VBASE_BN_LIB;
            goto err;
        }
        vb->default_g = gN->g;
        vb->default_N = gN->N;
        gN = NULL;
    }
    error_code = SRP_NO_ERROR;

 err:
    /*
     * there may be still some leaks to fix, if this fails, the application
     * terminates most likely
     */

    if (gN != NULL) {
        OPENSSL_free(gN->id);
        OPENSSL_free(gN);
    }

    SRP_user_pwd_free(user_pwd);

    TXT_DB_free(tmpdb);
    BIO_free_all(in);

    sk_SRP_gN_free(SRP_gN_tab);

    return error_code;

}