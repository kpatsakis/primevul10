static int core_cmd_callback (void *user, const char *cmd) {
    RCore *core = (RCore *)user;
    return r_core_cmd0 (core, cmd);
}