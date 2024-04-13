    long HttpIo::HttpImpl::getFileLength()
    {
        Exiv2::Dictionary response;
        Exiv2::Dictionary request;
        std::string errors;
        request["server"] = hostInfo_.Host;
        request["page"  ] = hostInfo_.Path;
        if (hostInfo_.Port != "") request["port"] = hostInfo_.Port;
        request["verb"]   = "HEAD";
        long serverCode = (long)http(request, response, errors);
        if (serverCode < 0 || serverCode >= 400 || errors.compare("") != 0) {
            throw Error(55, "Server", serverCode);
        }

        Exiv2::Dictionary_i lengthIter = response.find("Content-Length");
        return (lengthIter == response.end()) ? -1 : atol((lengthIter->second).c_str());
    }