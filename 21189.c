    FileIo::~FileIo()
    {
        close();
        delete p_;
    }