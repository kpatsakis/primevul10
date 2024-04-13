    void SshIo::SshImpl::writeRemote(const byte* data, size_t size, long from, long to)
    {
        if (protocol_ == pSftp) throw Error(1, "not support SFTP write access.");

        //printf("ssh update size=%ld from=%ld to=%ld\n", (long)size, from, to);
        assert(isMalloced_);

        std::string tempFile = hostInfo_.Path + ".exiv2tmp";
        std::string response;
        std::stringstream ss;
        // copy the head (byte 0 to byte fromByte) of original file to filepath.exiv2tmp
        ss  << "head -c " << from
            << " "   << hostInfo_.Path
            << " > " << tempFile;
        std::string cmd = ss.str();
        if (ssh_->runCommand(cmd, &response) != 0) {
            throw Error(1, "SSH: Unable to cope the head of file to temp");
        }

        // upload the data (the byte ranges which are different between the original
        // file and the new file) to filepath.exiv2datatemp
        if (ssh_->scp(hostInfo_.Path + ".exiv2datatemp", data, size) != 0) {
            throw Error(1, "SSH: Unable to copy file");
        }

        // concatenate the filepath.exiv2datatemp to filepath.exiv2tmp
        cmd = "cat " + hostInfo_.Path + ".exiv2datatemp >> " + tempFile;
        if (ssh_->runCommand(cmd, &response) != 0) {
            throw Error(1, "SSH: Unable to copy the rest");
        }

        // copy the tail (from byte toByte to the end of file) of original file to filepath.exiv2tmp
        ss.str("");
        ss  << "tail -c+" << (to + 1)
            << " "   << hostInfo_.Path
            << " >> "   << tempFile;
        cmd = ss.str();
        if (ssh_->runCommand(cmd, &response) != 0) {
            throw Error(1, "SSH: Unable to copy the rest");
        }

        // replace the original file with filepath.exiv2tmp
        cmd = "mv " + tempFile + " " + hostInfo_.Path;
        if (ssh_->runCommand(cmd, &response) != 0) {
            throw Error(1, "SSH: Unable to copy the rest");
        }

        // remove filepath.exiv2datatemp
        cmd = "rm " + hostInfo_.Path + ".exiv2datatemp";
        if (ssh_->runCommand(cmd, &response) != 0) {
            throw Error(1, "SSH: Unable to copy the rest");
        }
    }