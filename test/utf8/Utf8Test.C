/*
 * Copyright (C) 2009 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include <boost/test/unit_test.hpp>

#include <Wt/WString>
#include <Wt/WStringUtil>
#include <iostream>

namespace {
  bool endswith(const std::string &s, const std::string &suffix)
  {
    return s.size() >= suffix.size() &&
           s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
  }
}

BOOST_AUTO_TEST_CASE( Utf8_test1 )
{
#ifndef WT_NO_STD_WSTRING
#ifndef WT_WIN32
  std::wstring w = L"This costs 100\x20AC (greek \x0194 special \x103A7)";
  Wt::WString ws = w;
  // std::cerr << ws.toUTF8() << std::endl;
  BOOST_REQUIRE(ws.value() == w);
  BOOST_REQUIRE(ws.toUTF8().length() == w.length() + 2 + 1 + 3);
#else
  std::wstring w = L"This costs 100\x20AC (greek \x0194)";
  Wt::WString ws = w;
  // std::cerr << ws.toUTF8() << std::endl;
  BOOST_REQUIRE(ws.value() == w);
  BOOST_REQUIRE(ws.toUTF8().length() == w.length() + 2 + 1);
#endif
#endif
}

BOOST_AUTO_TEST_CASE( Utf8_test2 )
{
  /*
   * This test is broken on FreeBSD < 11 and was broken on macOS before Sierra.
   * There was a bug in their libc.
   *
   * See: https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=209907
   */
#if !(defined(__FreeBSD__) && __FreeBSD__ < 11)
#ifndef WT_NO_STD_WSTRING
  std::wstring w = L"This costs 100\x20AC (greek \x0194)";

  Wt::WString ws = w;
  std::string s = ws.narrow();

  // The following will work only if locale is classic.
  // If locale is UTF8, we have the original back.
  BOOST_REQUIRE(s == "This costs 100? (greek ?)" || s == ws.toUTF8());
#endif
#endif
}

BOOST_AUTO_TEST_CASE( Utf8_test3 )
{
  std::locale sys_locale("");
  std::locale other_locale = sys_locale;
  if (!(endswith(sys_locale.name(), ".UTF-8") ||
        endswith(sys_locale.name(), ".utf8"))) {
    try {
      other_locale = std::locale("en_US.utf8");
    } catch (const std::runtime_error&) {
      return; // Give up
    }
  }

  std::wstring w = L"\x20AC\x20AC\x20AC\x20AC (greek \x0194)";

  Wt::WString ws = w;

  std::locale l(std::locale("C"), other_locale,
		std::locale::collate | std::locale::ctype);

  std::string s = ws.narrow(l);

  BOOST_REQUIRE(s == ws.toUTF8());
}
 
BOOST_AUTO_TEST_CASE( Utf8_test4 )
{
  std::string u8 = "euro\xe2\x82\xac greek \xc6\x94 special \xf0\x90\x8e\xa7)";
  std::string u8a = "euro\xe2\x82\xac gree";
  std::string u8b = "euro\xe2\x82\xac greek \xc6\x94";
  std::string u8c = "euro\xe2\x82\xac greek \xc6\x94 special \xf0\x90\x8e\xa7";
  std::string u8d = "k \xc6\x94 special \xf0\x90\x8e\xa7)";
  std::string u8e = "special \xf0\x90\x8e\xa7";
  std::string u8f = ")";
  std::string ss;
  // Check if UTF8substr works as advertised
  ss = Wt::UTF8Substr(u8, 0, -1);
  BOOST_REQUIRE(ss == u8);

  ss = Wt::UTF8Substr(u8, 0, 24);
  BOOST_REQUIRE(ss == u8);

  ss = Wt::UTF8Substr(u8, 0, 23);
  BOOST_REQUIRE(ss == u8c);

  ss = Wt::UTF8Substr(u8, 0, 10);
  BOOST_REQUIRE(ss == u8a);

  ss = Wt::UTF8Substr(u8, 0, 13);
  BOOST_REQUIRE(ss == u8b);

  ss = Wt::UTF8Substr(u8, 10, -1);
  BOOST_REQUIRE(ss == u8d);

  ss = Wt::UTF8Substr(u8, 14, 9);
  BOOST_REQUIRE(ss == u8e);

  ss = Wt::UTF8Substr(u8, 23, 9);
  BOOST_REQUIRE(ss == u8f);

}

// Run this test with address sanitizer or valgrind
// There was an issue in Wt::narrow that caused a heap buffer overflow.
BOOST_AUTO_TEST_CASE( Utf8_test5 )
{
  std::locale sys_locale("");
  std::locale other_locale = sys_locale;
  if (!(endswith(sys_locale.name(), ".UTF-8") ||
        endswith(sys_locale.name(), ".utf8"))) {
    try {
      other_locale = std::locale("en_US.utf8");
    } catch (const std::runtime_error&) {
      return; // Give up
    }
  }

  std::locale l(std::locale("C"), other_locale,
                std::locale::collate | std::locale::ctype);

  std::wstring s1 = L"\U0001F638\U0001F631";
  std::string s2 = "\xF0\x9F\x98\xB8\xF0\x9F\x98\xB1";

  BOOST_REQUIRE(Wt::narrow(s1, l) == s2);
}
