stale_route_check_trigger(struct multi_context *m)
{
    struct timeval null;
    CLEAR(null);
    return event_timeout_trigger(&m->stale_routes_check_et, &null, ETT_DEFAULT);
}