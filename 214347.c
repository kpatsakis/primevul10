AP_DECLARE(int) ap_exists_config_define(const char *name)
{
    return ap_array_str_contains(ap_server_config_defines, name);
}