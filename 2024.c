int Socket::drainChunk(int timeout)    // reads chunks until 0 len chunk or timeout
{
    char buff[32000];
    int tot_size = 0;
    int csize = 1;
    while (csize > 0) {
        csize = readChunk(buff,32000, timeout);
        if (!(csize > -1 )) {
#ifdef CHUNKDEBUG
            std::cerr << thread_id << "drainChunk - error" << std::endl;
#endif
            return -1;
        }
        tot_size += csize;
    }
#ifdef CHUNKDEBUG
    std::cerr << thread_id << "drainChunk  tot_size=" << tot_size << std::endl;
#endif
    return tot_size;
}