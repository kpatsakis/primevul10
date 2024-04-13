TEST_P(JSITest, PropNameIDTest) {
  // This is a little weird to test, because it doesn't really exist
  // in JS yet.  All I can do is create them, compare them, and
  // receive one as an argument to a HostObject.

  PropNameID quux = PropNameID::forAscii(rt, "quux1", 4);
  PropNameID movedQuux = std::move(quux);
  EXPECT_EQ(movedQuux.utf8(rt), "quux");
  movedQuux = PropNameID::forAscii(rt, "quux2");
  EXPECT_EQ(movedQuux.utf8(rt), "quux2");
  PropNameID copiedQuux = PropNameID(rt, movedQuux);
  EXPECT_TRUE(PropNameID::compare(rt, movedQuux, copiedQuux));

  EXPECT_TRUE(PropNameID::compare(rt, movedQuux, movedQuux));
  EXPECT_TRUE(PropNameID::compare(
      rt, movedQuux, PropNameID::forAscii(rt, std::string("quux2"))));
  EXPECT_FALSE(PropNameID::compare(
      rt, movedQuux, PropNameID::forAscii(rt, std::string("foo"))));
  uint8_t utf8[] = {0xF0, 0x9F, 0x86, 0x97};
  PropNameID utf8PropNameID = PropNameID::forUtf8(rt, utf8, sizeof(utf8));
  EXPECT_EQ(utf8PropNameID.utf8(rt), u8"\U0001F197");
  EXPECT_TRUE(PropNameID::compare(
      rt, utf8PropNameID, PropNameID::forUtf8(rt, utf8, sizeof(utf8))));
  PropNameID nonUtf8PropNameID = PropNameID::forUtf8(rt, "meow");
  EXPECT_TRUE(PropNameID::compare(
      rt, nonUtf8PropNameID, PropNameID::forAscii(rt, "meow")));
  EXPECT_EQ(nonUtf8PropNameID.utf8(rt), "meow");
  PropNameID strPropNameID =
      PropNameID::forString(rt, String::createFromAscii(rt, "meow"));
  EXPECT_TRUE(PropNameID::compare(rt, nonUtf8PropNameID, strPropNameID));

  auto names = PropNameID::names(
      rt, "Ala", std::string("ma"), PropNameID::forAscii(rt, "kota"));
  EXPECT_EQ(names.size(), 3);
  EXPECT_TRUE(
      PropNameID::compare(rt, names[0], PropNameID::forAscii(rt, "Ala")));
  EXPECT_TRUE(
      PropNameID::compare(rt, names[1], PropNameID::forAscii(rt, "ma")));
  EXPECT_TRUE(
      PropNameID::compare(rt, names[2], PropNameID::forAscii(rt, "kota")));
}