static int test_ifdefine_section(cmd_parms *cmd, const char *arg)
{
    return ap_exists_config_define(arg);
}