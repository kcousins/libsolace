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
 *	@file		test/test_string.cpp
 *	@author		abbyssoul
 *
 ******************************************************************************/
#include <solace/string.hpp>  // Class being tested
#include <solace/exception.hpp>

#include <gtest/gtest.h>

#include <cstring>

using namespace Solace;

using array_size_t = ArrayView<String>::size_type;
const char* kSomeConstString = "Some static string";


TEST(TestString, testConstruction_null) {
    {   // NullPointer smoke test
        const char* nullCString = nullptr;

        EXPECT_NO_THROW(makeString(nullCString));
    }


    // Null string is null, cap
    /*
    EXPECT_TRUE(nullString.empty());
    EXPECT_TRUE(nullString.equals(String::Null));

    EXPECT_TRUE(String::Null.equals(nullCString));
    EXPECT_EQ(0, String::Null.compareTo(nullCString));
    EXPECT_EQ(false, nullString.contains(nullCString));

    EXPECT_EQ(nullCString, nullString.c_str());
    EXPECT_EQ(nullString, nullString.concat(nullCString));

    {
        String const v("Some random string");

        EXPECT_TRUE(!v.equals(String::Null));
        EXPECT_TRUE(!v.equals(nullCString));

        EXPECT_TRUE(v.indexOf(nullCString).isNone());
        EXPECT_TRUE(v.lastIndexOf(nullCString).isNone());
        EXPECT_EQ(0, v.compareTo(nullCString));
        EXPECT_EQ(false, v.contains(nullCString));
        EXPECT_EQ(v, v.concat(nullCString));
        EXPECT_EQ(v, nullString.concat(v));
    }*/
}

/**
* Test construction calls
*/
TEST(TestString, testDefaultConstruction) {
    EXPECT_TRUE(String{}.equals(String::Empty));
    EXPECT_TRUE(String{}.empty());
}

TEST(TestString, testCStrConstruction) {
    const char* source = "some cstr source";
    auto const  cstrFromTo = makeString(source + 5, 4);
    EXPECT_TRUE(cstrFromTo.equals("cstr"));
}


TEST(TestString, testMoveConstruction) {
    auto cstr = makeString(kSomeConstString);
    EXPECT_TRUE(cstr.equals(kSomeConstString));

    String const moved = std::move(cstr);
    EXPECT_TRUE(cstr.empty());
    EXPECT_FALSE(cstr.equals(kSomeConstString));
    EXPECT_TRUE(moved.equals(kSomeConstString));

    auto const  strCopy = makeString(moved);
    EXPECT_TRUE(strCopy.equals(kSomeConstString));
    EXPECT_TRUE(moved.equals(kSomeConstString));
}

/**
    * Tests assignment
    */
TEST(TestString, testMoveAssignment) {
    String str1;
    String substr;
    EXPECT_TRUE(str1.empty());

    {
        String str2 = makeString("some string");
        str1 = std::move(str2);
        substr = makeString(str1.substring(5, 8));
    }

    EXPECT_EQ(StringLiteral("some string"), str1);
    EXPECT_EQ(StringLiteral("str"), substr);
}

/**
    * @brief Test string equality functions.
    */
TEST(TestString, testEquality) {
    static const char* source1 = "some test string";
    static const char* source2 = "some other test string";
    static const char* source3 = "some test string";

    auto const str1 = makeString(source1);
    auto const str2 = makeString(source2);
    auto const str3 = makeString(source3);
    auto const str4 = makeString(str3);

    // Test if It is reflexive
    EXPECT_TRUE(String::Empty.equals(String::Empty));
    EXPECT_TRUE(str1.equals(str1));
    EXPECT_TRUE(str1.equals(source1));
    EXPECT_TRUE(str1 == source1);
    EXPECT_TRUE(str1 == str1);

    EXPECT_TRUE(str2.equals(source2));
    EXPECT_TRUE(str2 == source2);

    EXPECT_TRUE(str1 != source2);
    EXPECT_TRUE(str2 != str1);
    EXPECT_TRUE(str1 != str2);

    // Test if It is symmetric
    EXPECT_TRUE(str1.equals(source3));
    EXPECT_TRUE(str3.equals(source1));
    EXPECT_TRUE(str1.equals(str3));
    EXPECT_TRUE(str3.equals(str1));

    // Test if It is transitive
    EXPECT_TRUE(str1.equals(str3));
    EXPECT_TRUE(str3.equals(str4));
    EXPECT_TRUE(str4.equals(str1));

    EXPECT_TRUE(str1 == source3);
    EXPECT_TRUE(str3 == source1);

    EXPECT_TRUE(str1 == str3);
    EXPECT_TRUE(str3 == str1);

    EXPECT_EQ(str1, str3);
    EXPECT_EQ(str3, str1);
}

TEST(TestString, testContains) {
    String const source = makeString("Hello, world!  ");
    String const world = makeString("world");

    EXPECT_TRUE(source.contains(world));
    EXPECT_TRUE(source.contains('!'));
    EXPECT_TRUE(!source.contains("a"));
}

TEST(TestString, testLength) {
    String const source = makeString("");
    String const world = makeString("world");
    // FIXME: Add utf9 example

    EXPECT_EQ(0, source.length());
    EXPECT_EQ(5, world.length());
}


TEST(TestString, testReplace) {
    String const source = makeString("attraction{holder}");
    String const value = makeString("VALUE");

    EXPECT_EQ(source,                                   makeStringReplace(source, value, "{holder}"));
    EXPECT_EQ(StringLiteral("aXXracXion{holder}"),      makeStringReplace(source, 't', 'X'));
    EXPECT_EQ(StringLiteral("aWORDraction{holder}"),    makeStringReplace(source, "tt", "WORD"));
    EXPECT_EQ(StringLiteral("attractionVALUE"),         makeStringReplace(source, "{holder}", value));
}


TEST(TestString, testSplit) {
    String const dest0 = makeString("boo");
    String const dest1 = makeString("and");
    String const dest2 = makeString("foo");
    String const source = makeString("boo:and:foo");

    {   // Normal split
        std::vector<String> result;
        result.reserve(3);

        source.split(":", [&result](StringView bit) {
            result.emplace_back(makeString(bit));
        });

        EXPECT_EQ(static_cast<array_size_t>(3), result.size());
        EXPECT_EQ(dest0, result[0]);
        EXPECT_EQ(dest1, result[1]);
        EXPECT_EQ(dest2, result[2]);
    }

    {   // No splitting token in the string
        std::vector<String> result;
        dest0.split(':', [&result](StringView bit) {
            result.emplace_back(makeString(bit));
        });

        EXPECT_EQ(static_cast<array_size_t>(1), result.size());
        EXPECT_EQ(dest0, result[0]);
    }

    {   // No splitting token in the string
        std::vector<String> result;
        source.split("/", [&result](StringView bit) {
            result.emplace_back(makeString(bit));
        });

        EXPECT_EQ(static_cast<array_size_t>(1), result.size());
        EXPECT_EQ(source, result[0]);
    }

    {   // Split with empty token
        std::vector<String> result;
        makeString(":foo").split(":", [&result](StringView bit) {
            result.emplace_back(makeString(bit));
        });

        EXPECT_EQ(static_cast<array_size_t>(2), result.size());
        EXPECT_EQ(String::Empty, result[0]);
        EXPECT_EQ(dest2, result[1]);
    }
}

TEST(TestString, testIndexOf) {
    String const source = makeString("Hello, World! Good bye, World ");
    String const world = makeString("World");

    // Happy case:
    EXPECT_EQ(static_cast<String::size_type>(7), source.indexOf(world).get());
    EXPECT_EQ(static_cast<String::size_type>(12), source.indexOf('!').get());
    EXPECT_EQ(static_cast<String::size_type>(24), source.indexOf(world, 12).get());
    EXPECT_EQ(static_cast<String::size_type>(19), source.indexOf("bye", 3).get());

    // Fail cases:
    EXPECT_TRUE(source.indexOf("awesome").isNone());
    EXPECT_TRUE(source.indexOf("World", source.length() - 3).isNone());
    EXPECT_TRUE(source.indexOf("World", source.length() + 3).isNone());
    EXPECT_TRUE(source.indexOf('!', source.length() - 3).isNone());
    EXPECT_TRUE(source.indexOf('!', source.length()).isNone());
    EXPECT_TRUE(source.indexOf('!', source.length() + 25).isNone());

    EXPECT_TRUE(world.indexOf(source).isNone());
    EXPECT_TRUE(world.indexOf("Some very long and obscure string??").isNone());
    EXPECT_TRUE(world.indexOf(source, world.size() + 3).isNone());
    EXPECT_TRUE(world.indexOf("Some very long and obscure string??", world.size() + 3).isNone());

    EXPECT_TRUE(world.indexOf('/').isNone());
    EXPECT_TRUE(world.indexOf('!', 3321).isNone());
    EXPECT_TRUE(world.indexOf('!', source.length() + 25).isNone());
}

TEST(TestString, testLastIndexOf) {
    String const source = makeString("Hello, World! Good bye, World - and again!");
    String const world = makeString("World");

    EXPECT_EQ(static_cast<String::size_type>(24), source.lastIndexOf(world).get());
    EXPECT_EQ(static_cast<String::size_type>(41), source.lastIndexOf('!').get());
    EXPECT_EQ(static_cast<String::size_type>(24), source.lastIndexOf(world, 12).get());
    EXPECT_EQ(static_cast<String::size_type>(19), source.lastIndexOf("bye", 12).get());

    // Fail case:
    EXPECT_TRUE(source.lastIndexOf('!', source.length()).isNone());
    EXPECT_TRUE(source.lastIndexOf('!', source.length() + 25).isNone());
    EXPECT_TRUE(world.lastIndexOf(source).isNone());
    EXPECT_TRUE(world.lastIndexOf("Some very long and obscure string??").isNone());
    EXPECT_TRUE(world.lastIndexOf(source, world.size() + 3).isNone());
    EXPECT_TRUE(world.lastIndexOf("Some very long and obscure string??", world.size() + 3).isNone());

    EXPECT_TRUE(world.lastIndexOf('/').isNone());
}

TEST(TestString, testConcat) {
    String const hello = makeString("Hello");
    String const space = makeString(", ");
    String const world = makeString("world!");
    String const target = makeString("Hello, world!");

    EXPECT_EQ(hello, makeString(String::Empty, hello));
    EXPECT_EQ(hello, makeString(hello, String::Empty));

    EXPECT_EQ(target, makeString(hello, space, world));
    EXPECT_EQ(target, makeString(hello, StringLiteral(", world!")));
}

/**
    * @see String::substring
    */
TEST(TestString, testSubstring) {
    String const source = makeString("Hello, World! Good bye, World - and again!");
    String const world = makeString("World");
    String const bye = makeString("bye");
    String const and_again = makeString("and again!");

    // Identity
    EXPECT_EQ(world, world.substring(0));

    // From = To == Empty string
    EXPECT_TRUE(world.substring(1, 1).empty());
    EXPECT_TRUE(world.substring(3, 3).empty());

    EXPECT_EQ(world, source.substring(7, 12));
    EXPECT_EQ(and_again, source.substring(source.indexOf(and_again).get()));

    auto const byeIndex = source.indexOf(bye).get();
    EXPECT_EQ(bye, source.substring(byeIndex, byeIndex + bye.length()));

    // Saturation
    EXPECT_EQ(StringLiteral("Good bye, World - and again!"),
                            source.substring(source.indexOf("Good").get(), 1042));
    EXPECT_TRUE(source.substring(1042).empty());
    EXPECT_TRUE(source.substring(1042, 2048).empty());
}

/**
    * @see String::trim
    */
TEST(TestString, testTrim) {
    String testString;

    EXPECT_TRUE(testString.empty());
    EXPECT_TRUE(testString.trim().empty());

    // Total trim
    EXPECT_TRUE(makeString("   ").trim().empty());

    // Trim identity
    {
        const StringLiteral trimmed("Hello, world!");
        String const toTrim = makeString("Hello, world!");
        testString = makeString(toTrim.trim());
        EXPECT_TRUE(testString == toTrim);
        EXPECT_EQ(trimmed, testString);
    }

    // Trim start
    {
        const StringLiteral trimmed("Hello, world!");
        String const toTrim = makeString(" Hello, world!");
        testString = makeString(toTrim.trim());
        EXPECT_TRUE(testString != toTrim);
        EXPECT_EQ(trimmed, testString);
    }

    // Trim both
    {
        const StringLiteral trimmed("Hello, world!");
        String const toTrim = makeString("  Hello, world!  ");
        testString = makeString(toTrim.trim());
        EXPECT_TRUE(testString != toTrim);
        EXPECT_EQ(trimmed, testString);
    }

    // Trim End
    {
        const StringLiteral trimmed("Hello, world !");
        String const toTrim = makeString("Hello, world !  ");
        testString = makeString(toTrim.trim());
        EXPECT_TRUE(testString != toTrim);
        EXPECT_EQ(trimmed, testString);
    }
}

/**
    * Test string's toLowerCase
TEST(TestString, testToLowerCase) {
    // Lower case -> lower case
    {
        String const lowerCaseSource("hello there");
        String const lowerCase_toLower = lowerCaseSource.toLowerCase();

        // Identity:
        EXPECT_EQ(lowerCaseSource, lowerCase_toLower);
    }

    // Mixed case -> lower case
    {
        String const mixedCaseSource("Hello Out There!");
        String const mixedCaseSource_toLower = mixedCaseSource.toLowerCase();

        EXPECT_TRUE(mixedCaseSource != mixedCaseSource_toLower);
        EXPECT_TRUE(mixedCaseSource_toLower.equals("hello out there!"));
    }

    // Upper case -> lower case
    {
        String const upperCaseSource("THERE@OUT*HELLO*&^%");
        String const upperCaseSource_toLower = upperCaseSource.toLowerCase();

        EXPECT_TRUE(upperCaseSource != upperCaseSource_toLower);
        EXPECT_TRUE(upperCaseSource_toLower.equals("there@out*hello*&^%"));
    }
}
*/

/**
    * Test string's toUpperCase
TEST(TestString, testToUpperCase) {
    // Lower case -> upper case
    {
        String const lowerCaseSource("hello@there-out*&%!1");
        String const lowerCase_toUpper = lowerCaseSource.toUpperCase();

        EXPECT_TRUE(lowerCaseSource != lowerCase_toUpper);
        EXPECT_EQ(String("HELLO@THERE-OUT*&%!1"), lowerCase_toUpper);
    }

    // Mixed case -> lower case
    {
        String const mixedCaseSource("Hello @ Out^&There!");
        String const mixedCaseSource_toUpper = mixedCaseSource.toUpperCase();

        EXPECT_TRUE(mixedCaseSource != mixedCaseSource_toUpper);
        EXPECT_EQ(String("HELLO @ OUT^&THERE!"), mixedCaseSource_toUpper);
    }

    // Upper case -> lower case
    {
        String const upperCaseSource("THERE@OUT*HELLO*&^%");
        String const upperCaseSource_toUpper = upperCaseSource.toUpperCase();

        // Identity
        EXPECT_EQ(upperCaseSource, upperCaseSource_toUpper);
    }
}
*/


/**
    * Test string's 'startsWith'
    */
TEST(TestString, testStartsWith) {
    String const source = makeString("Hello, world out there!");
    String const hello = makeString("Hello");
    String const there = makeString("there!");
    String const overlong = makeString("Hello, world out there! And here!");

    EXPECT_FALSE(String::Empty.startsWith('H'));
    EXPECT_FALSE(String::Empty.startsWith("Something"));
    EXPECT_TRUE(source.startsWith(String::Empty));

    EXPECT_TRUE(source.startsWith('H'));
    EXPECT_TRUE(source.startsWith(hello));

    EXPECT_FALSE(source.startsWith(there));
    EXPECT_FALSE(source.startsWith(overlong));
    EXPECT_FALSE(source.startsWith("Some very long statement that can't possibly feet"));
}

/**
    * Test string's 'endsWith'
    */
TEST(TestString, testEndsWith) {
    String const source = makeString("Hello, world out there !");
    String const hello = makeString("Hello");
    String const there = makeString("there !");
    String const overlong = makeString("Hello, world out there ! And here!");

    EXPECT_TRUE(!String::Empty.endsWith('x'));
    EXPECT_TRUE(!String::Empty.endsWith("Something"));
    EXPECT_TRUE(source.endsWith(String::Empty));
    EXPECT_TRUE(source.endsWith('!'));
    EXPECT_TRUE(source.endsWith(source));
    EXPECT_TRUE(!source.endsWith(hello));
    EXPECT_TRUE(source.endsWith(there));
    EXPECT_TRUE(!source.endsWith(overlong));
    EXPECT_TRUE(!source.endsWith("Some very long statement that can't possibly feet"));
}

/**
    * Test string's 'hashCode' method.
    */
TEST(TestString, testHashCode) {
    String const testString1 = makeString("Hello otu there");
    String const testString2 = makeString("Hello out there");

    EXPECT_NE(testString1.hashCode(), 0);
    EXPECT_NE(testString2.hashCode(), 0);
    EXPECT_NE(testString1, testString2);
    EXPECT_NE(testString1.hashCode(), testString2.hashCode());
}

/**
    * Test string's 'format' methods.
    */
/*
void testFormat() {
    String const hello = "Hello";

    // Format
    // Tests Identity:
    EXPECT_EQ(hello, String::format(hello));
    EXPECT_EQ(hello, String::format("%s", hello));

    EXPECT_EQ(hello, String::format(hello));

    EXPECT_EQ(String("1"), String::format("%d", 1));
    EXPECT_EQ(String("CSTR"), String::format("%s", "CSTR"));
    EXPECT_EQ(String("Hello2World"), String::format("%s", hello, 2, "World"));

    EXPECT_EQ(String::Empty, String::format("", hello));

    EXPECT_EQ(String("xxx"), String::format("xxx", hello));
    EXPECT_THROW(String::format("xxx%s", hello, "bbb"), std::runtime_error);
    EXPECT_THROW(String::format("%sxxx%szxc", hello), std::runtime_error);

    // Append format
    EXPECT_NO_THROW(testString1.appendFormat(strT("hello")));
    EXPECT_EQ(String(strT("hello")), testString1);

    EXPECT_NO_THROW(testString1.appendFormat(strT("%d"), 1));
    EXPECT_EQ(String(strT("hello1")), testString1);

    EXPECT_NO_THROW(testString1.appendFormat(strT(": %s"), testString2));
    EXPECT_EQ(String(strT("hello1: Hello, world!")), testString1);

    // Printf
    EXPECT_NO_THROW(testString1.printf(strT("hello")));
    EXPECT_EQ(String(strT("hello")), testString1);

    EXPECT_NO_THROW(testString1.printf(strT("%d"), 1));
    EXPECT_EQ(String(strT("1")), testString1);

    EXPECT_NO_THROW(testString1.printf(strT("%s"), str1));
    EXPECT_EQ(String(str1), testString1);
}
*/

TEST(TestString, testToString) {
    String const ident = makeString(kSomeConstString);

    EXPECT_EQ(ident,  ident.toString());
}


TEST(TestString, test_iterable_forEach) {
//        String const ident(kSomeConstString);

//        int summ = 0;
//        for (auto c : ident.view()) {
//            summ += c;
//        }

//        ident.forEach([&summ](const Char& c) {
//            summ -= c.getValue();
//        });

//        EXPECT_EQ(0, summ);
}
