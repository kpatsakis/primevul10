static int test_ifdirective_section(cmd_parms *cmd, const char *arg)
{
    return ap_exists_directive(cmd->temp_pool, arg);
}