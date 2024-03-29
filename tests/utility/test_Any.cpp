// Copyright 2009 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "../catch.hpp"

#include "rkcommon/math/box.h"
#include "rkcommon/math/vec.h"
#include "rkcommon/utility/Any.h"

using namespace rkcommon::utility;
using namespace rkcommon::math;

// Helper types ///////////////////////////////////////////////////////////////

struct OSPObject_T
{
};
using OSPObject = OSPObject_T *;

// Helper functions ///////////////////////////////////////////////////////////

template <typename T>
inline void verify_value(const Any &v, const T &correctValue)
{
  REQUIRE(v.valid());
  REQUIRE(v.is<T>());
  REQUIRE(v.get<T>() == correctValue);
}

template <typename T>
inline void test_interface(T testValue, T testValue2)
{
  Any v;
  REQUIRE(!v.valid());

  SECTION("Can make valid by construction")
  {
    Any v2(testValue);
    verify_value<T>(v2, testValue);
  }

  SECTION("Can make valid by calling operator=()")
  {
    v = testValue;
    verify_value<T>(v, testValue);
  }

  SECTION("Can make valid by copy construction")
  {
    v = testValue;
    Any v2(v);
    verify_value<T>(v2, testValue);
  }

  SECTION("Two objects with same value are equal if constructed the same")
  {
    v      = testValue;
    Any v2 = testValue;
    REQUIRE(v == v2);
  }

  SECTION("Two objects with same value are equal if assigned from another")
  {
    v      = testValue;
    Any v2 = testValue2;
    v      = v2;
    REQUIRE(v == v2);
  }

  SECTION("Two objects with different values are not equal")
  {
    v      = testValue;
    Any v2 = testValue2;
    REQUIRE(v != v2);
  }
}

// Tests //////////////////////////////////////////////////////////////////////

TEST_CASE("Any 'int' type behavior", "[Any]")
{
  test_interface<int>(5, 7);
}

TEST_CASE("Any 'float' type behavior", "[Any]")
{
  test_interface<float>(1.f, 2.f);
}

TEST_CASE("Any 'bool' type behavior", "[Any]")
{
  test_interface<bool>(true, false);
}

TEST_CASE("Any 'vec3f' type behavior", "[Any]")
{
  test_interface<vec3f>({1.f, 1.f, 1.f}, {2.f, 3.f, 4.f});
}

TEST_CASE("Any 'vec2f' type behavior", "[Any]")
{
  test_interface<vec2f>({1.f, 1.f}, {3.f, 4.f});
}

TEST_CASE("Any 'vec2i' type behavior", "[Any]")
{
  test_interface<vec2i>({1, 1}, {3, 4});
}

TEST_CASE("Any 'box3f' type behavior", "[Any]")
{
  test_interface<box3f>({{1.f, 1.f, 1.f}, {2.f, 2.f, 2.f}},
                        {{3.f, 4.f, 5.f}, {6.f, 7.f, 8.f}});
}

TEST_CASE("Any 'string' type behavior", "[Any]")
{
  test_interface<std::string>("Hello", "World");
}

TEST_CASE("Any 'OSPObject' type behavior", "[Any]")
{
  // NOTE(jda) - we just need some phony pointer addresses to test Any,
  //             no need to hand it "real" OSPRay objects...
  void *val1 = nullptr;
  void *val2 = nullptr;
  val1       = &val1;
  val2       = &val2;

  test_interface<OSPObject>(static_cast<OSPObject>(val1),
                            static_cast<OSPObject>(val2));
}
