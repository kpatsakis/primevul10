TEST_CASE("Invalid UTF-8 text test", "[general]")
{
    std::string s = "a <- '";
    s += static_cast<char>(0xe8); // Make invalid utf8 text...

    parser parser(s.c_str());

    bool ret = parser;
    REQUIRE(ret == false);
}