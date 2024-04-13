nasm_set_limit(const char *limit, const char *valstr)
{
    int i;
    int64_t val;
    bool rn_error;
    int errlevel;

    for (i = 0; i <= LIMIT_MAX; i++) {
        if (!nasm_stricmp(limit, limit_info[i].name))
            break;
    }
    if (i > LIMIT_MAX) {
        if (passn == 0)
            errlevel = ERR_WARNING|ERR_NOFILE|ERR_USAGE;
        else
            errlevel = ERR_WARNING|ERR_PASS1|ERR_WARN_UNKNOWN_PRAGMA;
        nasm_error(errlevel, "unknown limit: `%s'", limit);
        return DIRR_ERROR;
    }

    if (!nasm_stricmp(valstr, "unlimited")) {
        val = LIMIT_MAX_VAL;
    } else {
        val = readnum(valstr, &rn_error);
        if (rn_error || val < 0) {
            if (passn == 0)
                errlevel = ERR_WARNING|ERR_NOFILE|ERR_USAGE;
            else
                errlevel = ERR_WARNING|ERR_PASS1|ERR_WARN_BAD_PRAGMA;
            nasm_error(errlevel, "invalid limit value: `%s'", limit);
            return DIRR_ERROR;
        }
        if (val > LIMIT_MAX_VAL)
            val = LIMIT_MAX_VAL;
    }

    nasm_limit[i] = val;
    return DIRR_OK;
}