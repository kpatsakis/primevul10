void Socket::close()
{
    if (isssl) {
        stopSsl();
    }
    BaseSocket::close();
}