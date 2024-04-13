    long MemIo::write(const byte* data, long wcount)
    {
        p_->reserve(wcount);
        assert(p_->isMalloced_);
        std::memcpy(&p_->data_[p_->idx_], data, wcount);
        p_->idx_ += wcount;
        return wcount;
    }