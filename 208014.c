int squidclamav_post_init_service(ci_service_xdata_t * srv_xdata, struct ci_server_conf *server_conf)
{

    if (squidguard == NULL) return 0;

    ci_debug_printf(1, "DEBUG squidclamav_post_init_service: opening pipe to %s\n", squidguard);

    if (create_pipe(squidguard) == 1) {
	return 0;
    }

    return 1;
}