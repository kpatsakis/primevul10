    DataBuf FileIo::read(long rcount)
    {
        assert(p_->fp_ != 0);
        if ( (size_t) rcount > size() ) throw Error(57);
        DataBuf buf(rcount);
        long readCount = read(buf.pData_, buf.size_);
        buf.size_ = readCount;
        return buf;
    }