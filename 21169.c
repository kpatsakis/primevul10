    std::wstring ReplaceStringInPlace(std::wstring subject, const std::wstring& search,
                          const std::wstring& replace) {
        std::wstring::size_type pos = 0;
        while((pos = subject.find(search, pos)) != std::wstring::npos) {
             subject.replace(pos, search.length(), replace);
             pos += replace.length();
        }
        return subject;
    }