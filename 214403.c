static int test_ifmod_section(cmd_parms *cmd, const char *arg)
{
    return find_module(cmd->server, arg) != NULL;
}