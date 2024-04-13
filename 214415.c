static const char *set_etag_bits(cmd_parms *cmd, void *mconfig,
                                 const char *args_p)
{
    core_dir_config *cfg;
    etag_components_t bit;
    char action;
    char *token;
    const char *args;
    int valid;
    int first;
    int explicit;

    cfg = (core_dir_config *)mconfig;

    args = args_p;
    first = 1;
    explicit = 0;
    while (args[0] != '\0') {
        action = '*';
        bit = ETAG_UNSET;
        valid = 1;
        token = ap_getword_conf(cmd->temp_pool, &args);
        if ((*token == '+') || (*token == '-')) {
            action = *token;
            token++;
        }
        else {
            /*
             * The occurrence of an absolute setting wipes
             * out any previous relative ones.  The first such
             * occurrence forgets any inherited ones, too.
             */
            if (first) {
                cfg->etag_bits = ETAG_UNSET;
                cfg->etag_add = ETAG_UNSET;
                cfg->etag_remove = ETAG_UNSET;
                first = 0;
            }
        }

        if (ap_cstr_casecmp(token, "None") == 0) {
            if (action != '*') {
                valid = 0;
            }
            else {
                cfg->etag_bits = bit = ETAG_NONE;
                explicit = 1;
            }
        }
        else if (ap_cstr_casecmp(token, "All") == 0) {
            if (action != '*') {
                valid = 0;
            }
            else {
                explicit = 1;
                cfg->etag_bits = bit = ETAG_ALL;
            }
        }
        else if (ap_cstr_casecmp(token, "Size") == 0) {
            bit = ETAG_SIZE;
        }
        else if ((ap_cstr_casecmp(token, "LMTime") == 0)
                 || (ap_cstr_casecmp(token, "MTime") == 0)
                 || (ap_cstr_casecmp(token, "LastModified") == 0)) {
            bit = ETAG_MTIME;
        }
        else if (ap_cstr_casecmp(token, "INode") == 0) {
            bit = ETAG_INODE;
        }
        else {
            return apr_pstrcat(cmd->pool, "Unknown keyword '",
                               token, "' for ", cmd->cmd->name,
                               " directive", NULL);
        }

        if (! valid) {
            return apr_pstrcat(cmd->pool, cmd->cmd->name, " keyword '",
                               token, "' cannot be used with '+' or '-'",
                               NULL);
        }

        if (action == '+') {
            /*
             * Make sure it's in the 'add' list and absent from the
             * 'subtract' list.
             */
            cfg->etag_add |= bit;
            cfg->etag_remove &= (~ bit);
        }
        else if (action == '-') {
            cfg->etag_remove |= bit;
            cfg->etag_add &= (~ bit);
        }
        else {
            /*
             * Non-relative values wipe out any + or - values
             * accumulated so far.
             */
            cfg->etag_bits |= bit;
            cfg->etag_add = ETAG_UNSET;
            cfg->etag_remove = ETAG_UNSET;
            explicit = 1;
        }
    }

    /*
     * Any setting at all will clear the 'None' and 'Unset' bits.
     */

    if (cfg->etag_add != ETAG_UNSET) {
        cfg->etag_add &= (~ ETAG_UNSET);
    }

    if (cfg->etag_remove != ETAG_UNSET) {
        cfg->etag_remove &= (~ ETAG_UNSET);
    }

    if (explicit) {
        cfg->etag_bits &= (~ ETAG_UNSET);

        if ((cfg->etag_bits & ETAG_NONE) != ETAG_NONE) {
            cfg->etag_bits &= (~ ETAG_NONE);
        }
    }

    return NULL;
}