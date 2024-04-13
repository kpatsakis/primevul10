char *get_current_dir_name() {
    static char buf[MAX_PATH] = ".";
    getcwd(buf, sizeof(buf));
    return buf;
}