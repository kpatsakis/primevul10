    void SshIo::SshImpl::getDataByRange(long lowBlock, long highBlock, std::string& response)
    {
        if (protocol_ == pSftp) {
            if (sftp_seek(fileHandler_, (uint32_t) (lowBlock * blockSize_)) < 0) throw Error(1, "SFTP: unable to sftp_seek");
            size_t buffSize = (highBlock - lowBlock + 1) * blockSize_;
            char* buffer = new char[buffSize];
            long nBytes = (long) sftp_read(fileHandler_, buffer, buffSize);
            if (nBytes < 0) throw Error(1, "SFTP: unable to sftp_read");
            response.assign(buffer, buffSize);
            delete[] buffer;
        } else {
            std::stringstream ss;
            if (lowBlock > -1 && highBlock > -1) {
                ss  << "dd if=" << hostInfo_.Path
                    << " ibs=" << blockSize_
                    << " skip=" << lowBlock
                    << " count=" << (highBlock - lowBlock) + 1<< " 2>/dev/null";
            } else {
                ss  << "dd if=" << hostInfo_.Path
                    << " ibs=" << blockSize_
                    << " 2>/dev/null";
            }
            std::string cmd = ss.str();
            if (ssh_->runCommand(cmd, &response) != 0) {
                throw Error(1, "Unable to get data by range.");
            }
        }
    }