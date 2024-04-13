static int log_constant_item(const ap_errorlog_info *info, const char *arg,
                             char *buf, int buflen)
{
    char *end = apr_cpystrn(buf, arg, buflen);
    return end - buf;
}