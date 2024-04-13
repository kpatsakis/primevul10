bool Socket::readyForOutput()
{
    //if (!isssl) {
        return BaseSocket::readyForOutput();
    //}

    //cant do this on a blocking ssl socket as far as i can work out

    //return true;
}