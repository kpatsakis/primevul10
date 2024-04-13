    SshIo::SshImpl::SshImpl(const std::wstring& wurl, size_t blockSize):Impl(wurl, blockSize)
    {
        std::string url;
        url.assign(wurl.begin(), wurl.end());
        path_ = url;

        hostInfo_ = Exiv2::Uri::Parse(url);
        Exiv2::Uri::Decode(hostInfo_);

        // remove / at the beginning of the path
        if (hostInfo_.Path[0] == '/') {
            hostInfo_.Path = hostInfo_.Path.substr(1);
        }
        ssh_ = new SSH(hostInfo_.Host, hostInfo_.Username, hostInfo_.Password, hostInfo_.Port);

        if (protocol_ == pSftp) {
            ssh_->getFileSftp(hostInfo_.Path, fileHandler_);
            if (fileHandler_ == NULL) throw Error(1, "Unable to open the file");
        } else {
            fileHandler_ = NULL;
        }
    }