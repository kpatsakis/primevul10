int Socket::readChunk( char *buffin, int maxlen, int timeout){
    if (chunk_to_read == 0)     // need to read chunk size
    {
        char size[40];
        ieof = false;
        int len = getLine(size, 38, timeout);
        if (len < 2) {   // min valid length is 2 i.e.  "0\r"
            chunkError = true;
            return -1;
        }
#ifdef CHUNKDEBUG
        std::cerr << thread_id << "readChunk  size=" << size << std::endl;
#endif
        String l = size;
        l.chop();
        String t = l.before(";");
        if (t.length() > 0) {
            if (l.endsWith("; ieof")) {
                ieof = true;
            }
            l = t;
        }
        chunk_to_read = l.hexToInteger();
#ifdef CHUNKDEBUG
        std::cerr << thread_id << "readChunk  chunk_to_read =" << chunk_to_read << std::endl;
#endif
    }

    int clen = chunk_to_read;
    if (clen > maxlen) {
        clen = maxlen;
    }
    int rc = 0;
#ifdef CHUNKDEBUG
    std::cerr << thread_id << "readChunk  max_read =" << clen << std::endl;
#endif

    if(clen == 0) {
        chunked_trailer = "";
        char trailer[32000];
        int len = 3;
        while( len > 2) {
            len = getLine(trailer, 31900, timeout);
            if (len > 2) {
                chunked_trailer += trailer;
                chunked_trailer += "\n";
            }
        }
        return 0;
    }

    if (clen > 0) {
        rc = readFromSocketn(buffin, clen, 0, timeout);
#ifdef CHUNKDEBUG
        std::cerr << thread_id << "readChunk  read " << rc << std::endl;
#endif
        if (rc < 0) {
            chunkError = true;
            return -1;
        }
        chunk_to_read -= rc;
    }
    if (chunk_to_read > 0)    // there is more to read in this chunk - so do not check for trailing \r\n
        return rc;
    char ts[2];
    int len = readFromSocketn(ts, 2, 0, timeout);
    if (len == 2 && ts[0] == '\r' && ts[1] == '\n') {
        return rc;
    } else {
        chunkError = true;
#ifdef CHUNKDEBUG
        std::cerr << thread_id << "readChunk - tail in error" << std::endl;
#endif
        return -1;
    }
}