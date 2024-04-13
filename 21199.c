    void RemoteIo::transfer(BasicIo& src)
    {
        if (src.open() != 0) {
            throw Error(1, "unable to open src when transferring");
        }
        write(src);
        src.close();
    }