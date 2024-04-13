route_quota_exceeded(const struct multi_instance *mi)
{
    struct gc_arena gc = gc_new();
    msg(D_ROUTE_QUOTA, "MULTI ROUTE: route quota (%d) exceeded for %s (see --max-routes-per-client option)",
        mi->context.options.max_routes_per_client,
        multi_instance_string(mi, false, &gc));
    gc_free(&gc);
}