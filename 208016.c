int squidclamav_init_service(ci_service_xdata_t * srv_xdata,
                           struct ci_server_conf *server_conf)
{
    unsigned int xops;

    ci_debug_printf(1, "DEBUG squidclamav_init_service: Going to initialize squidclamav\n");

    squidclamav_xdata = srv_xdata;
    set_istag(squidclamav_xdata);
    ci_service_set_preview(srv_xdata, 1024);
    ci_service_enable_204(srv_xdata);
    ci_service_set_transfer_preview(srv_xdata, "*");

    xops = CI_XCLIENTIP | CI_XSERVERIP;
    xops |= CI_XAUTHENTICATEDUSER | CI_XAUTHENTICATEDGROUPS;
    ci_service_set_xopts(srv_xdata, xops);
 

    /*Initialize object pools*/
    AVREQDATA_POOL = ci_object_pool_register("av_req_data_t", sizeof(av_req_data_t));

    if(AVREQDATA_POOL < 0) {
	 ci_debug_printf(0, "FATAL squidclamav_init_service: error registering object_pool av_req_data_t\n");
	 return 0;
    }

    /* Reload configuration command */
    register_command("squidclamav:cfgreload", MONITOR_PROC_CMD | CHILDS_PROC_CMD, cfgreload_command);

     
    /*********************
       read config files
     ********************/
    clamd_curr_ip = (char *) malloc (sizeof (char) * 128);
    memset(clamd_curr_ip, 0, sizeof(clamd_curr_ip));

    if (load_patterns() == 0) {
	return 0;
    }

    return 1;
}