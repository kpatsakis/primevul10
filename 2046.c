bool Socket::writeChunkTrailer( String &trailer) {
    std::string hexs ("0\r\n");
#ifdef CHUNKDEBUG
    std::cerr << thread_id << "writeChunk  size=" << hexs << std::endl;
#endif
    if(writeString(hexs.c_str()) && writeToSocket(trailer.c_str(),trailer.length(),0,timeout) && writeString("\r\n"))
        return true;
    return false;
};