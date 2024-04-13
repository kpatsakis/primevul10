static const char *start_cond_section(cmd_parms *cmd, void *mconfig, const char *arg)
{
    const char *endp = ap_strrchr_c(arg, '>');
    int result, not = (arg[0] == '!');
    test_cond_section_fn testfn = (test_cond_section_fn)cmd->info;
    const char *arg1;

    if (endp == NULL) {
        return unclosed_directive(cmd);
    }

    arg = apr_pstrmemdup(cmd->temp_pool, arg, endp - arg);

    if (not) {
        arg++;
    }

    arg1 = ap_getword_conf(cmd->temp_pool, &arg);

    if (!arg1[0]) {
        return missing_container_arg(cmd);
    }

    result = testfn(cmd, arg1);

    if ((!not && result) || (not && !result)) {
        ap_directive_t *parent = NULL;
        ap_directive_t *current = NULL;
        const char *retval;

        retval = ap_build_cont_config(cmd->pool, cmd->temp_pool, cmd,
                                      &current, &parent, cmd->cmd->name);
        *(ap_directive_t **)mconfig = current;
        return retval;
    }
    else {
        *(ap_directive_t **)mconfig = NULL;
        return ap_soak_end_container(cmd, cmd->cmd->name);
    }
}