    void HttpIo::HttpImpl::getDataByRange(long lowBlock, long highBlock, std::string& response)
    {
        Exiv2::Dictionary responseDic;
        Exiv2::Dictionary request;
        request["server"] = hostInfo_.Host;
        request["page"  ] = hostInfo_.Path;
        if (hostInfo_.Port != "") request["port"] = hostInfo_.Port;
        request["verb"]   = "GET";
        std::string errors;
        if (lowBlock > -1 && highBlock > -1) {
            std::stringstream ss;
            ss << "Range: bytes=" << lowBlock * blockSize_  << "-" << ((highBlock + 1) * blockSize_ - 1) << "\r\n";
            request["header"] = ss.str();
        }

        long serverCode = (long)http(request, responseDic, errors);
        if (serverCode < 0 || serverCode >= 400 || errors.compare("") != 0) {
            throw Error(55, "Server", serverCode);
        }
        response = responseDic["body"];
    }