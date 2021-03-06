// ======================================================================== //
// Copyright 2009-2015 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#ifndef __RTCORE_H__
#define __RTCORE_H__

#include <stddef.h>
#include <sys/types.h>

#if defined(_WIN32)
#if defined(_M_X64)
typedef long long ssize_t;
#else
typedef int ssize_t;
#endif
#endif

#ifndef RTCORE_API
#if defined(_WIN32) && !defined(ENABLE_STATIC_LIB)
#  define RTCORE_API extern "C" __declspec(dllimport) 
#else
#  define RTCORE_API extern "C"
#endif
#endif

#ifdef _WIN32
#  define RTCORE_ALIGN(...) __declspec(align(__VA_ARGS__))
#else
#  define RTCORE_ALIGN(...) __attribute__((aligned(__VA_ARGS__)))
#endif

#ifdef __GNUC__
  #define RTCORE_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
  #define RTCORE_DEPRECATED __declspec(deprecated)
#else
  #define RTCORE_DEPRECATED
#endif

#include "rtcore_scene.h"
#include "rtcore_geometry.h"
#include "rtcore_geometry_user.h"

/*! \file rtcore.h Defines the Embree Ray Tracing Kernel API for C and C++ 

   This file defines the Embree ray tracing kernel API for C and
   C++. The user is supposed to include this file, and alternatively
   the rtcore_ray.h file, but none of the other .h files in this
   folder. */

/*! \{ */

/*! \brief Initializes the Embree ray tracing core

  Initializes the ray tracing core and passed some configuration
  string. The configuration string allows to configure implementation
  specific parameters. If this string is NULL, a default configuration
  is used. The following configuration flags are supported by the
  Embree implementation of the API:
  
  threads = num,       // sets the number of threads to use (default is to use all threads)
  verbose = num,       // sets verbosity level (default is 0)

  If Embree is started on an unsupported CPU, rtcInit will fail and
  set the RTC_UNSUPPORTED_CPU error code.
  
*/
RTCORE_API void rtcInit(const char* cfg = NULL);

/*! \brief Shuts down Embree. 

  Shuts down the ray tracing core. After shutdown, all scene handles
  are invalid, and invoking any API call except rtcInit is not
  allowed. The application should invoke this call before
  terminating. It is safe to call rtcInit again after an rtcExit
  call. */
RTCORE_API void rtcExit();

/*! \brief Error codes returned by the rtcGetError function. */
enum RTCError {
  RTC_NO_ERROR = 0,          //!< No error has been recorded.
  RTC_UNKNOWN_ERROR = 1,     //!< An unknown error has occured.
  RTC_INVALID_ARGUMENT = 2,  //!< An invalid argument is specified
  RTC_INVALID_OPERATION = 3, //!< The operation is not allowed for the specified object.
  RTC_OUT_OF_MEMORY = 4,     //!< There is not enough memory left to execute the command.
  RTC_UNSUPPORTED_CPU = 5,   //!< The CPU is not supported as it does not support SSE2.
  RTC_CANCELLED = 6,         //!< The user has cancelled the operation through the RTC_PROGRESS_MONITOR_FUNCTION callback
};

/*! \brief Returns the value of the per-thread error flag. 

  If an error occurs this flag is set to an error code if it stores no
  previous error. The rtcGetError function reads and returns the
  currently stored error and clears the error flag again. */
RTCORE_API RTCError rtcGetError();

/*! \brief Type of error callback function. */
typedef void (*RTC_ERROR_FUNCTION)(const RTCError code, const char* str);

/*! \brief Sets a callback function that is called whenever an error occurs. */
RTCORE_API void rtcSetErrorFunction(RTC_ERROR_FUNCTION func);

/*! \brief Type of memory consumption callback function. */
typedef bool (*RTC_MEMORY_MONITOR_FUNCTION)(const ssize_t bytes, const bool post);

/*! \brief Sets the memory consumption callback function which is
 *  called before or after the library allocates or frees memory. */
RTCORE_API void rtcSetMemoryMonitorFunction(RTC_MEMORY_MONITOR_FUNCTION func);

/*! \brief Implementation specific (do not call).

  This function is implementation specific and only for debugging
  purposes. Do not call it. */
RTCORE_API RTCORE_DEPRECATED void rtcDebug(); // FIXME: remove

/*! \brief Helper to easily combing scene flags */
inline RTCSceneFlags operator|(const RTCSceneFlags a, const RTCSceneFlags b) {
  return (RTCSceneFlags)((size_t)a | (size_t)b);
}

/*! \brief Helper to easily combing algorithm flags */
inline RTCAlgorithmFlags operator|(const RTCAlgorithmFlags a, const RTCAlgorithmFlags b) {
  return (RTCAlgorithmFlags)((size_t)a | (size_t)b);
}

/*! \brief Helper to easily combing geometry flags */
inline RTCGeometryFlags operator|(const RTCGeometryFlags a, const RTCGeometryFlags b) {
  return (RTCGeometryFlags)((size_t)a | (size_t)b);
}

/*! \} */

#endif
