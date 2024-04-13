bool Socket::bcheckForInput(int timeout)
{
    if (!isssl) {
        return BaseSocket::bcheckForInput(timeout);
    }
    return true;
}