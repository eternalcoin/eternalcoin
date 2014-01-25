#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ETC");
    BOOST_CHECK(GetBoolArg("-ETC"));
    BOOST_CHECK(GetBoolArg("-ETC", false));
    BOOST_CHECK(GetBoolArg("-ETC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ETCo"));
    BOOST_CHECK(!GetBoolArg("-ETCo", false));
    BOOST_CHECK(GetBoolArg("-ETCo", true));

    ResetArgs("-ETC=0");
    BOOST_CHECK(!GetBoolArg("-ETC"));
    BOOST_CHECK(!GetBoolArg("-ETC", false));
    BOOST_CHECK(!GetBoolArg("-ETC", true));

    ResetArgs("-ETC=1");
    BOOST_CHECK(GetBoolArg("-ETC"));
    BOOST_CHECK(GetBoolArg("-ETC", false));
    BOOST_CHECK(GetBoolArg("-ETC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noETC");
    BOOST_CHECK(!GetBoolArg("-ETC"));
    BOOST_CHECK(!GetBoolArg("-ETC", false));
    BOOST_CHECK(!GetBoolArg("-ETC", true));

    ResetArgs("-noETC=1");
    BOOST_CHECK(!GetBoolArg("-ETC"));
    BOOST_CHECK(!GetBoolArg("-ETC", false));
    BOOST_CHECK(!GetBoolArg("-ETC", true));

    ResetArgs("-ETC -noETC");  // -ETC should win
    BOOST_CHECK(GetBoolArg("-ETC"));
    BOOST_CHECK(GetBoolArg("-ETC", false));
    BOOST_CHECK(GetBoolArg("-ETC", true));

    ResetArgs("-ETC=1 -noETC=1");  // -ETC should win
    BOOST_CHECK(GetBoolArg("-ETC"));
    BOOST_CHECK(GetBoolArg("-ETC", false));
    BOOST_CHECK(GetBoolArg("-ETC", true));

    ResetArgs("-ETC=0 -noETC=0");  // -ETC should win
    BOOST_CHECK(!GetBoolArg("-ETC"));
    BOOST_CHECK(!GetBoolArg("-ETC", false));
    BOOST_CHECK(!GetBoolArg("-ETC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ETC=1");
    BOOST_CHECK(GetBoolArg("-ETC"));
    BOOST_CHECK(GetBoolArg("-ETC", false));
    BOOST_CHECK(GetBoolArg("-ETC", true));

    ResetArgs("--noETC=1");
    BOOST_CHECK(!GetBoolArg("-ETC"));
    BOOST_CHECK(!GetBoolArg("-ETC", false));
    BOOST_CHECK(!GetBoolArg("-ETC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ETC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ETC", "eleven"), "eleven");

    ResetArgs("-ETC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ETC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ETC", "eleven"), "");

    ResetArgs("-ETC=");
    BOOST_CHECK_EQUAL(GetArg("-ETC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ETC", "eleven"), "");

    ResetArgs("-ETC=11");
    BOOST_CHECK_EQUAL(GetArg("-ETC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ETC", "eleven"), "11");

    ResetArgs("-ETC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ETC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ETC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ETC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ETC", 0), 0);

    ResetArgs("-ETC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ETC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ETC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ETC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ETC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ETC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ETC");
    BOOST_CHECK_EQUAL(GetBoolArg("-ETC"), true);

    ResetArgs("--ETC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ETC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noETC");
    BOOST_CHECK(!GetBoolArg("-ETC"));
    BOOST_CHECK(!GetBoolArg("-ETC", true));
    BOOST_CHECK(!GetBoolArg("-ETC", false));

    ResetArgs("-noETC=1");
    BOOST_CHECK(!GetBoolArg("-ETC"));
    BOOST_CHECK(!GetBoolArg("-ETC", true));
    BOOST_CHECK(!GetBoolArg("-ETC", false));

    ResetArgs("-noETC=0");
    BOOST_CHECK(GetBoolArg("-ETC"));
    BOOST_CHECK(GetBoolArg("-ETC", true));
    BOOST_CHECK(GetBoolArg("-ETC", false));

    ResetArgs("-ETC --noETC");
    BOOST_CHECK(GetBoolArg("-ETC"));

    ResetArgs("-noETC -ETC"); // ETC always wins:
    BOOST_CHECK(GetBoolArg("-ETC"));
}

BOOST_AUTO_TEST_SUITE_END()
