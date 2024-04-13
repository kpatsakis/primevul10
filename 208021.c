void set_istag(ci_service_xdata_t * srv_xdata)
{
     char istag[SERVICE_ISTAG_SIZE + 1];


     snprintf(istag, SERVICE_ISTAG_SIZE, "-%d-%s-%d%d",1, "squidclamav", 1, 0);
     istag[SERVICE_ISTAG_SIZE] = '\0';
     ci_service_set_istag(srv_xdata, istag);
     ci_debug_printf(2, "DEBUG set_istag: setting istag to %s\n", istag);
}