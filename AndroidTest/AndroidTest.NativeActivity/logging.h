#pragma once

#define APP_NAME "AndroidTest"

#define LOGW(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_WARN, APP_NAME, __VA_ARGS__))

#define LOGI(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_INFO, APP_NAME, __VA_ARGS__))

#define LOGE(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_ERROR, APP_NAME, __VA_ARGS__))

/* For debug builds, always enable the debug traces in this library */
#ifndef NDEBUG
#define LOGV(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_VERBOSE, APP_NAME, __VA_ARGS__))
#else
#define LOGV(...) ((void)0)
#endif
