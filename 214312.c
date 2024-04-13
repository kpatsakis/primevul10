static const char *set_runtime_dir(cmd_parms *cmd, void *dummy, const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);

    if (err != NULL) {
        return err;
    }

    if ((apr_filepath_merge((char**)&ap_runtime_dir, NULL,
                            ap_server_root_relative(cmd->temp_pool, arg),
                            APR_FILEPATH_TRUENAME, cmd->pool) != APR_SUCCESS)
        || !ap_is_directory(cmd->temp_pool, ap_runtime_dir)) {
        return "DefaultRuntimeDir must be a valid directory, absolute or relative to ServerRoot";
    }

    return NULL;
}