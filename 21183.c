    long CurlIo::write(BasicIo& src)
    {
        if (p_->protocol_ == pHttp || p_->protocol_ == pHttps) {
            return RemoteIo::write(src);
        } else {
            throw Error(1, "doesnt support write for this protocol.");
        }
    }