    void CurlIo::CurlImpl::writeRemote(const byte* data, size_t size, long from, long to)
    {
        std::string scriptPath(getEnv(envHTTPPOST));
        if (scriptPath == "") {
            throw Error(1, "Please set the path of the server script to handle http post data to EXIV2_HTTP_POST environmental variable.");
        }

        Exiv2::Uri hostInfo = Exiv2::Uri::Parse(path_);

        // add the protocol and host to the path
        std::size_t protocolIndex = scriptPath.find("://");
        if (protocolIndex == std::string::npos) {
            if (scriptPath[0] != '/') scriptPath = "/" + scriptPath;
            scriptPath = hostInfo.Protocol + "://" + hostInfo.Host + scriptPath;
        }

        curl_easy_reset(curl_); // reset all options
        curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1L); // no progress meter please
        //curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1); // debugging mode
        curl_easy_setopt(curl_, CURLOPT_URL, scriptPath.c_str());
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);


        // encode base64
        size_t encodeLength = ((size + 2) / 3) * 4 + 1;
        char* encodeData = new char[encodeLength];
        base64encode(data, size, encodeData, encodeLength);
        // url encode
        char* urlencodeData = urlencode(encodeData);
        delete[] encodeData;
        std::stringstream ss;
        ss << "path="       << hostInfo.Path << "&"
           << "from="       << from          << "&"
           << "to="         << to            << "&"
           << "data="       << urlencodeData;
        std::string postData = ss.str();
        delete[] urlencodeData;

        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, postData.c_str());
        // Perform the request, res will get the return code.
        CURLcode res = curl_easy_perform(curl_);

        if(res != CURLE_OK) {
            throw Error(1, curl_easy_strerror(res));
        } else {
            long serverCode;
            curl_easy_getinfo (curl_, CURLINFO_RESPONSE_CODE, &serverCode);
            if (serverCode >= 400 || serverCode < 0) {
                throw Error(55, "Server", serverCode);
            }
        }
    }