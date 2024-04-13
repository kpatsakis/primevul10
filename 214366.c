AP_DECLARE(const char *) ap_get_server_description(void)
{
    return server_description ? server_description :
        AP_SERVER_BASEVERSION " (" PLATFORM ")";
}