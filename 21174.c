    long CurlIo::write(const byte* data, long wcount)
    {
        if (p_->protocol_ == pHttp || p_->protocol_ == pHttps) {
            return RemoteIo::write(data, wcount);
        } else {
            throw Error(1, "doesnt support write for this protocol.");
        }
    }