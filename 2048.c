bool Socket::breadyForOutput(int timeout)
{
    //if (!isssl) {
        return BaseSocket::breadyForOutput(timeout);
    //}
    //return true;
}