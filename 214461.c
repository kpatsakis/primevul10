AP_DECLARE(int) ap_allow_options(request_rec *r)
{
    core_dir_config *conf =
      (core_dir_config *)ap_get_core_module_config(r->per_dir_config);

    return conf->opts;
}