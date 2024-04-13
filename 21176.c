    long RemoteIo::read(byte* buf, long rcount)
    {
        assert(p_->isMalloced_);
        if (p_->eof_) return 0;
        p_->totalRead_ += rcount;

        size_t allow     = EXV_MIN(rcount, (long)( p_->size_ - p_->idx_));
        size_t lowBlock  =  p_->idx_         /p_->blockSize_;
        size_t highBlock = (p_->idx_ + allow)/p_->blockSize_;

        // connect to the remote machine & populate the blocks just in time.
        p_->populateBlocks(lowBlock, highBlock);
        byte* fakeData = (byte*) std::calloc(p_->blockSize_, sizeof(byte));
        if (!fakeData) {
            throw Error(1, "Unable to allocate data");
        }

        size_t iBlock = lowBlock;
        size_t startPos = p_->idx_ - lowBlock*p_->blockSize_;
        size_t totalRead = 0;
        do {
            byte* data = p_->blocksMap_[iBlock++].getData();
            if (data == NULL) data = fakeData;
            size_t blockR = EXV_MIN(allow, p_->blockSize_ - startPos);
            std::memcpy(&buf[totalRead], &data[startPos], blockR);
            totalRead += blockR;
            startPos = 0;
            allow -= blockR;
        } while(allow);

        if (fakeData) std::free(fakeData);

        p_->idx_ += (long) totalRead;
        p_->eof_ = (p_->idx_ == (long) p_->size_);

        return (long) totalRead;
    }