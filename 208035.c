void squidclamav_close_service()
{
    ci_debug_printf(1, "DEBUG squidclamav_close_service: clean all memory!\n");
    free_global();
    free_pipe();
    ci_object_pool_unregister(AVREQDATA_POOL);
}