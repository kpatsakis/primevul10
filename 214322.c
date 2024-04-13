AP_DECLARE(const char *) ap_get_server_banner(void)
{
    return server_banner ? server_banner : AP_SERVER_BASEVERSION;
}