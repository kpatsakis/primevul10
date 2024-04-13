int Socket::loopChunk(int timeout)    // reads chunks and sends back until 0 len chunk or timeout
{
    char buff[32000];
    int tot_size = 0;
    int csize = 1;
    while (csize > 0) {
        csize = readChunk(buff,32000, timeout);
        if (csize == 0)     // end chunk
        {
            if (!writeChunkTrailer(chunked_trailer))
            {
#ifdef CHUNKDEBUG
                std::cerr << thread_id << "loopChunk - error in writing chunk trailer" << std::endl;
#endif
                return -1;

            };
#ifdef CHUNKDEBUG
            std::cerr << thread_id << "loopChunk  tot_size=" << tot_size << std::endl;
#endif
            return tot_size;
        }
        if (!(csize > 0 && writeChunk(buff,csize,timeout))) {
#ifdef CHUNKDEBUG
            std::cerr << thread_id << "loopChunk - error" << std::endl;
#endif
            return -1;
        }
        tot_size += csize;
    }
    return -1;  // should never get here!
}