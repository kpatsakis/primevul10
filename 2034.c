bool Socket::writeChunk( char *buffout, int len, int timeout){
    std::stringstream stm;
    stm << std::hex << len;
    std::string hexs (stm.str());
    //int lw;
    hexs += "\r\n";
#ifdef NETDEBUG
    std::cerr << thread_id << "writeChunk  size=" << hexs << std::endl;
#endif
    if(writeString(hexs.c_str()) && writeToSocket(buffout,len,0,timeout) && writeString("\r\n"))
        return true;
    return false;
};