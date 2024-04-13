void CClient::ReachedMaxBuffer() {
    DEBUG(GetSockName() << " == ReachedMaxBuffer()");
    if (IsAttached()) {
        PutClient("ERROR :" + t_s("Closing link: Too long raw line"));
    }
    Close();
}