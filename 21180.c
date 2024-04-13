    CurlIo::CurlImpl::CurlImpl(const std::string& url, size_t blockSize):Impl(url, blockSize)
    {
        // init curl pointer
        curl_ = curl_easy_init();
        if(!curl_) {
            throw Error(1, "Uable to init libcurl.");
        }

        // The default block size for FTP is much larger than other protocols
        // the reason is that getDataByRange() in FTP always creates the new connection,
        // so we need the large block size to reduce the overhead of creating the connection.
        if (blockSize_ == 0) {
            blockSize_ = protocol_ == pFtp ? 102400 : 1024;
        }

        std::string timeout = getEnv(envTIMEOUT);
        timeout_ = atol(timeout.c_str());
        if (timeout_ == 0) {
            throw Error(1, "Timeout Environmental Variable must be a positive integer.");
        }
    }