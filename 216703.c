update_mstat_n_clients(const int n_clients)
{
#ifdef ENABLE_MEMSTATS
    if (mmap_stats)
    {
        mmap_stats->n_clients = n_clients;
    }
#endif
}