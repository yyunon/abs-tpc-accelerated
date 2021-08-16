//
// Created by Fabian Nonnenmacher on 07.05.20.
//
#pragma once

#include <iostream>
#include <fletcher/api.h>
#include <arrow/api.h>
#include <jni.h>


inline void exitWithError(const std::string &msg)
{
  std::cout << "ERROR: " << msg << std::endl;
  exit(-1);
}


template <typename error_t>
inline void ASSERT_FLETCHER_OK(error_t s)
{
  do
  {
    fletcher::Status _s = s;
    if (!_s.ok())
    {
      std::cout << "[FLETCHER ERROR]: " << _s.message << "\n";
      exit(-1);
    }

  } while (0);
}

template <typename error_t>
inline void ASSERT_OK(error_t s)
{
  do
  {
    ::arrow::Status _s = ::arrow::internal::GenericToStatus(s);
    if (!_s.ok())
    {
      std::cout << "[FLETCHER ERROR]:  " << _s.CodeAsString() << ": " << _s.message() << "\n";
      exit(-1);
    }
  } while (0);
}

template <typename error_msg, typename cond_t>
inline void ASSERT(cond_t condition, error_msg msg)
{
  do
  {
    if (!(condition))
    {
      exitWithError(msg);
    }
  } while (0);
}
