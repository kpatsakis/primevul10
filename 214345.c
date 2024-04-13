AP_DECLARE(int) ap_state_query(int query)
{
    switch (query) {
    case AP_SQ_MAIN_STATE:
        return ap_main_state;
    case AP_SQ_RUN_MODE:
        return ap_run_mode;
    case AP_SQ_CONFIG_GEN:
        return ap_config_generation;
    default:
        return AP_SQ_NOT_SUPPORTED;
    }
}