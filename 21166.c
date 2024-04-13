    byte* RemoteIo::mmap(bool /*isWriteable*/)
    {
        size_t nRealData = 0 ;
        if ( !bigBlock_ ) {
            size_t blockSize = p_->blockSize_;
            size_t blocks = (p_->size_ + blockSize -1)/blockSize ;
            bigBlock_   = new byte[blocks*blockSize] ;
            for ( size_t block = 0 ; block < blocks ; block ++ ) {
                void* p = p_->blocksMap_[block].getData();
                if  ( p ) {
                    nRealData += blockSize ;
                    memcpy(bigBlock_+(block*blockSize),p,blockSize);
                }
            }
#ifdef DEBUG
            std::cerr << "RemoteIo::mmap nRealData = " << nRealData << std::endl;
#endif
		}

        return bigBlock_;
    }