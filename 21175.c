    long SshIo::SshImpl::getFileLength()
    {
        long length = 0;
        if (protocol_ == pSftp) { // sftp
            sftp_attributes attributes = sftp_fstat(fileHandler_);
            length = (long)attributes->size;
        } else { // ssh
            std::string response;
            //std::string cmd = "stat -c %s " + hostInfo_.Path;
            std::string cmd = "declare -a x=($(ls -alt " + hostInfo_.Path + ")); echo ${x[4]}";
            if (ssh_->runCommand(cmd, &response) != 0) {
                throw Error(1, "Unable to get file length.");
            } else {
                length = atol(response.c_str());
                if (length == 0) {
                    throw Error(1, "File is empty or not found.");
                }
            }
        }
        return length;
    }