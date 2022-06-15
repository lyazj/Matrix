#pragma once

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <stdexcept>

#define TEST_PASSED do { \
  std::cout << "Passed: " << __func__ << std::endl; \
} while(0)

#define ASSERT_EXCEPTION(exc, stat) \
  try { \
    { stat } \
    assert(!"exception " #exc " not thrown"); \
  } \
  catch(const exc &) { }
