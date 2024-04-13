static QString passwordQuery(bool name)
{
    QString retval("");
    if (name)
    {
        retval = currentUser() + "\n";
    }
    return retval + glocale->translate("Enter Password");
}