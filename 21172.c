    CurlIo::CurlImpl::CurlImpl(const std::wstring& wurl, size_t blockSize):Impl(wurl, blockSize)
    {
        std::string url;
        url.assign(wurl.begin(), wurl.end());
        path_ = url;

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
    }