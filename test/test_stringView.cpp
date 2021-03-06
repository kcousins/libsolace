/*
*  Copyright 2016 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/*******************************************************************************
 * libSolace Unit Test Suit
 *	@file		test/test_stringView.cpp
 *	@author		abbyssoul
 *
 ******************************************************************************/
#include <solace/stringView.hpp>  // Class being tested
#include <solace/exception.hpp>
#include <solace/arrayView.hpp>

#include <gtest/gtest.h>

#include <cstring>

using namespace Solace;

using array_size_t = ArrayView<StringView>::size_type;



TEST(TestStringView, testConstructionFromNull) {
    const char* nullCString = nullptr;

    auto const s1 = StringView(nullCString, 0);
    EXPECT_TRUE(s1.empty());

    EXPECT_NO_THROW(StringView{nullCString});
    EXPECT_THROW(StringView nullString(nullCString, 16), Exception);
}

TEST(TestStringView, testConstructEmptyString) {
    StringView value;
    EXPECT_TRUE(value.empty());
    EXPECT_EQ(0, value.size());
    EXPECT_EQ(0, value.length());

    EXPECT_TRUE(StringView("").empty());
}

TEST(TestStringView, testConstructionUnsized) {
    const char *cstr = "world";
    EXPECT_TRUE(!StringView("Non-empty").empty());
    EXPECT_EQ(5, StringView(cstr).size());
    EXPECT_EQ(5, StringView(cstr).length());
    EXPECT_EQ(cstr, StringView(cstr).data());
}

TEST(TestStringView, testConstructionSized) {
    const char *cstr = "world";

    EXPECT_EQ(3, StringView(cstr, 3).length());
    EXPECT_EQ(3, StringView(&cstr[2], 3).length());
    EXPECT_EQ(cstr, StringView(cstr, 3).data());
    EXPECT_EQ(cstr + 2, StringView(&cstr[2], 3).data());
}

TEST(TestStringView, testAssignmentFromCstring) {
    StringView value("world");
    EXPECT_EQ(5, value.length());
    EXPECT_EQ(0, strcmp("world", value.data()));

    value = "Something else";
    EXPECT_EQ(14, value.size());
    EXPECT_EQ(0, strcmp("Something else", value.data()));
}


/**
    * @see StringView::operator=
    */
TEST(TestStringView, testAssignment) {
    StringView value("world");
    StringView other("Completely different value");

    value = other;

    EXPECT_EQ(26, value.length());
    EXPECT_EQ(0, strcmp("Completely different value", value.data()));
}


/**
* @see StringView::equals
* @see StringView::operator==
*/
TEST(TestStringView, testEquality) {
    StringView value1("hello");
    StringView value2("Completely different value");
    StringView value3("hello");

    EXPECT_TRUE(value1.equals("hello"));
    EXPECT_TRUE(!value1.equals("hello here"));
    EXPECT_TRUE(!value1.equals("here hello"));

    EXPECT_TRUE(value1.equals(value1));
    EXPECT_TRUE(!value1.equals(value2));
    EXPECT_TRUE(value1.equals(value3));
    EXPECT_TRUE(value3.equals(value1));

    EXPECT_EQ(value1, "hello");
    EXPECT_NE(value1, "hello here");
    EXPECT_NE(value1, "hi hello");

    EXPECT_TRUE(value1 == value1);
    EXPECT_TRUE(!(value1 == value2));
    EXPECT_TRUE(value1 != value2);
    EXPECT_TRUE(value1 == value3);
    EXPECT_TRUE(value3 == value1);
}

/**
* @see StringView::compareTo
* @see StringView::operator<
*/
TEST(TestStringView, testCompareTo) {
    EXPECT_TRUE(StringView{"aab"} > StringLiteral{"aaa"});
    EXPECT_TRUE(StringView{"aabc"} > StringLiteral{"aab"});

    EXPECT_TRUE(StringView{"baa"} < StringLiteral{"bbc"});
    EXPECT_TRUE(StringView{"bcd"} < StringLiteral{"bcd0x"});
}

/**
    * @see StringView::length
    */
TEST(TestStringView, testLength) {
    // FIXME: Add utf8 example

    EXPECT_EQ(0, StringView().length());
    EXPECT_EQ(0, StringView("").length());
    EXPECT_EQ(5, StringView("world").length());
}


/**
    * @see StringView::startsWith
    */
TEST(TestStringView, testStartsWith) {
    EXPECT_TRUE(StringView{}.startsWith('\0'));
    EXPECT_FALSE(StringView{}.startsWith('t'));
    EXPECT_TRUE(!StringView("Hello world").startsWith('\0'));
    EXPECT_TRUE(StringView("Hello world").startsWith('H'));

    EXPECT_TRUE(StringView{}.startsWith(StringView{}));
    EXPECT_TRUE(StringView{"Hello"}.startsWith(""));
    EXPECT_FALSE(StringView{}.startsWith("Hello"));
    EXPECT_TRUE(StringView("Hello world").startsWith("Hello"));
    EXPECT_TRUE(!StringView("Hello world").startsWith("world"));
    EXPECT_TRUE(!StringView("Some")
                    .startsWith("Some very long statement that can't possibly"));
}

/**
    * @see StringView::endsWith
    */
TEST(TestStringView, testEndsWith) {
    EXPECT_TRUE(StringView{}.endsWith('\0'));
    EXPECT_TRUE(!StringView("Hello world!").endsWith('\0'));
    EXPECT_TRUE(StringView("Hello world!").endsWith('!'));

    EXPECT_TRUE(StringView{}.endsWith(StringView{}));
    EXPECT_TRUE(StringView{"Hello"}.endsWith(""));
    EXPECT_TRUE(StringView("Hello world!").endsWith("world!"));
    EXPECT_TRUE(!StringView("Hello world").endsWith("hello"));
    EXPECT_TRUE(!StringView("Hello world")
                    .endsWith("Some very long statement that can't possibly"));
}

/**
* @see StringView::substring
*/
TEST(TestStringView, testSubstring) {
    const StringView source("Hello, world! Good bye, World - and again!");
    const StringView bye("bye");
    const StringView andAgain("and again!");

    // Identity
    EXPECT_EQ(StringView(), StringView().substring(0));
    EXPECT_EQ(source, source.substring(0));
    EXPECT_EQ(andAgain, source.substring(source.indexOf(andAgain).get()));

    EXPECT_EQ(StringView("world"), source.substring(7, 12));


    auto const byeIndex = source.indexOf(bye).get();
    EXPECT_EQ(bye, source.substring(byeIndex, byeIndex + bye.length()));

    // Saturation
    EXPECT_TRUE(StringView("hi").substring(13).empty());
    EXPECT_TRUE(StringView("hi").substring(13, 14).empty());
    EXPECT_TRUE(StringView("hi").substring(13, 3).empty());
    EXPECT_EQ(StringView("hi"), StringView("hi").substring(0, 8));
    EXPECT_TRUE(StringView("hi there").substring(4, 2).empty());
}


/**
* @see StringView::trim
*/
TEST(TestStringView, testTrim) {
    EXPECT_TRUE(StringView().trim().empty());

    // Total trim
    EXPECT_TRUE(StringView("   ").trim().empty());

    // Trim identity
    EXPECT_TRUE(StringView("Hello, world!").trim().equals("Hello, world!"));

    // Trim start
    EXPECT_TRUE(StringView("  Hello, world!").trim().equals("Hello, world!"));

    // Trim both
    EXPECT_TRUE(StringView(" Hello, world!   ").trim().equals("Hello, world!"));

    // Trim End
    EXPECT_TRUE(StringView("Hello, world!  ").trim().equals("Hello, world!"));
}


/**
    * @see StringView::indexOf
    */
TEST(TestStringView, testIndexOf) {
    const StringView src("Hello, world! $$Blarg");

    // Happy case:
    EXPECT_EQ('!', src[src.indexOf('!').get()]);
    EXPECT_EQ(12, src.indexOf('!').get());
    EXPECT_EQ(7, src.indexOf("world").get());
    EXPECT_EQ(3, StringView("hello").indexOf("lo").get());


    EXPECT_EQ(14, src.indexOf('$', 12).get());
    EXPECT_EQ(15, src.indexOf("$Bl", 3).get());

    // Not found case:
    EXPECT_TRUE(src.indexOf('!', 14).isNone());
    EXPECT_TRUE(src.indexOf("awesome").isNone());
    EXPECT_TRUE(src.indexOf("World", src.length() - 3).isNone());
    EXPECT_TRUE(StringView("hi").indexOf("hi, long string").isNone());

    // Fail case:
    EXPECT_TRUE(StringView("hi").indexOf('i', 5).isNone());
    EXPECT_TRUE(StringView("hi").indexOf("hi", 5).isNone());
}

/**
    * @see StringView::lastIndexOf
    */
TEST(TestStringView, testLastIndexOf) {
    const StringView source("Hello, World! Good bye, World - and again rld!");
    const StringView world("World");

    EXPECT_EQ('!', source[source.lastIndexOf('!').get()]);
    EXPECT_EQ(45, source.lastIndexOf('!').get());
    EXPECT_EQ(24, source.lastIndexOf(world).get());
    EXPECT_EQ(24, source.lastIndexOf(world, 12).get());
    EXPECT_EQ(42, source.lastIndexOf("rld!").get());

    // Not found case:
    EXPECT_TRUE(source.lastIndexOf('x').isNone());
    EXPECT_TRUE(source.lastIndexOf('x', 45).isNone());
    EXPECT_TRUE(source.lastIndexOf("awesome").isNone());
    EXPECT_TRUE(source.lastIndexOf(world, source.length() - 3).isNone());
    EXPECT_TRUE(StringView("hi, i,").lastIndexOf("i, long string").isNone());

    // Fail case:
    EXPECT_TRUE(StringView("hi").lastIndexOf('i', 5).isNone());
    EXPECT_TRUE(StringView("hi").lastIndexOf("hi", 5).isNone());
}

/**
    * @see StringView::contains
    */
TEST(TestStringView, testContains) {
    EXPECT_TRUE(StringView("Hello, world!").contains('e'));
    EXPECT_TRUE(StringView("Hello, world!").contains("world"));
    EXPECT_TRUE(!StringView("hi").contains('!'));
    EXPECT_TRUE(!StringView("hi").contains("hight"));
}

/**
    * @see StringView::hashCode
    */
TEST(TestStringView, testHashCode) {
    EXPECT_NE(StringView("Hello otu there").hashCode(), 0);

    EXPECT_TRUE(StringView("Hello otu there").hashCode() !=
                    StringView("Hello out there").hashCode());
}

TEST(TestStringView, testSplitByChar) {
    {
        int acc = 0;
        // Splitting empty string gives you 1 item in a collection - empty string
        StringView().split('x', [&acc](StringView ) {
            ++acc;
        });
        EXPECT_EQ(1, acc);
    }

    {   // Normal split
        std::vector<StringView> result;

        StringView("boo:and:foo").split(':', [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(3, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
        EXPECT_EQ(StringView("and"), result[1]);
        EXPECT_EQ(StringView("foo"), result[2]);
    }

    {   // Normal split 2
        std::vector<StringView> result;
        StringView("warning,performance,portability,")
                .split(',', [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(4, result.size());
        EXPECT_EQ(StringView("warning"), result[0]);
        EXPECT_EQ(StringView("performance"), result[1]);
        EXPECT_EQ(StringView("portability"), result[2]);
        EXPECT_EQ(StringView(), result[3]);
    }

    {   // Normal split with empty token in the middle
        std::vector<StringView> result;
        StringView("boo::foo").split(':', [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(3, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
        EXPECT_EQ(StringView(),      result[1]);
        EXPECT_EQ(StringView("foo"), result[2]);
    }

    {   // Normal split with empty token in the middle and in the end
        std::vector<StringView> result;
        result.reserve(4);

        StringView("boo::foo:").split(':', [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(4, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
        EXPECT_EQ(StringView(),      result[1]);
        EXPECT_EQ(StringView("foo"), result[2]);
        EXPECT_EQ(StringView(),      result[3]);
    }

    {   // No splitting token in the string
        std::vector<StringView> result;

        StringView("boo").split(':', [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(1, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
    }
    {   // Normal split with empty token in beggining
        std::vector<StringView> result;
        StringView(":boo").split(':', [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(2, result.size());
        EXPECT_EQ(StringView(),      result[0]);
        EXPECT_EQ(StringView("boo"), result[1]);
    }
}


TEST(TestStringView, testSplitByStringToken) {
    {
        int acc = 0;
        // Splitting empty string gives you 1 item in a collection - empty string
        StringView().split("tok", [&acc](StringView ) {
            ++acc;
        });
        EXPECT_EQ(1, acc);
    }

    {   // Narmal split
        std::vector<StringView> result;

        StringView("boo:!and:!foo").split(":!", [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(3, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
        EXPECT_EQ(StringView("and"), result[1]);
        EXPECT_EQ(StringView("foo"), result[2]);
    }
    {   // Narmal split
        std::vector<StringView> result;
        StringView("boo:!and:!").split(":!", [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(3, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
        EXPECT_EQ(StringView("and"), result[1]);
        EXPECT_EQ(StringView(), result[2]);
    }
    {   // Narmal split
        std::vector<StringView> result;
        StringView("boo:!:!foo:!").split(":!", [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(4, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
        EXPECT_EQ(StringView(),      result[1]);
        EXPECT_EQ(StringView("foo"), result[2]);
        EXPECT_EQ(StringView(),      result[3]);
    }
    {   // Narmal split
        std::vector<StringView> result;
        StringView(":x!boofoo:x!").split(":x!", [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(3, result.size());
        EXPECT_EQ(StringView(),         result[0]);
        EXPECT_EQ(StringView("boofoo"), result[1]);
        EXPECT_EQ(StringView(),         result[2]);
    }

    {   // No splitting token in the string
        std::vector<StringView> result;
        StringView("boo").split("other", [&result](StringView bit) {
            result.emplace_back(bit);
        });

        EXPECT_EQ(1, result.size());
        EXPECT_EQ(StringView("boo"), result[0]);
    }
}


TEST(TestStringView, splittingByEmptyToken) {
    {
        int acc = 0;

        auto const src = StringView{"this is some arbiterry string"};
        src.split("", [&acc](StringView segment) {
            acc += segment.size();
        });

        EXPECT_EQ(src.size(), acc);
    }
}
