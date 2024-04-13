static int core_upgrade_storage(request_rec *r)
{
    if ((r->method_number == M_OPTIONS) && r->uri && (r->uri[0] == '*') &&
        (r->uri[1] == '\0')) {
        return core_upgrade_handler(r);
    }
    return DECLINED;
}