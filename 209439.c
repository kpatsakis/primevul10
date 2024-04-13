int Jsi_NumberIsInfinity(Jsi_Number a) {
#if JSI__MUSL==1 || defined(__FreeBSD__) || defined(__WIN32)
    if (!isinf(a))
        return 0;
    return (a<0 ? -1 : 1);
#else
    return isinf(a);
#endif
}