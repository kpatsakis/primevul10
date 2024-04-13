    int RemoteIo::close()
    {
        if (p_->isMalloced_) {
            p_->eof_ = false;
            p_->idx_ = 0;
        }
#ifdef DEBUG
        std::cerr << "RemoteIo::close totalRead_ = " << p_->totalRead_ << std::endl;
#endif
		if ( bigBlock_ ) {
			delete [] bigBlock_;
			bigBlock_=NULL;
		}
        return 0;
    }