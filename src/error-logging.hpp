#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

// libobs logging - https://github.com/obsproject/obs-studio/blob/master/libobs/util/base.h
#include <util/base.h>

#define CAFF_LOG(severity, format, ...) \
	blog(severity, "[caffeine-obs-plugin] " format, ##__VA_ARGS__);

#define CAFF_LOG_DEBUG(format, ...) CAFF_LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#define CAFF_LOG_INFO(format, ...) CAFF_LOG(LOG_INFO, format, ##__VA_ARGS__)
#define CAFF_LOG_WARNING(format, ...) \
	CAFF_LOG(LOG_WARNING, format, ##__VA_ARGS__)
#define CAFF_LOG_ERROR(format, ...) CAFF_LOG(LOG_ERROR, format, ##__VA_ARGS__)

// Used to protect against changes in the underlying enums
#define ASSERT_MATCH(left, right)                                     \
	static_assert((left) == static_cast<decltype(left)>((right)), \
		      "Mismatch between " #left " and " #right)

#define IS_VALID_ENUM_VALUE(prefix, value) \
	((value) >= 0 && (value) <= (prefix##Last))

#define CAFF_CHECK(condition)                                                  \
	do {                                                                   \
		if (!(condition)) {                                            \
			CAFF_LOG_ERROR("Check failed [%s]: ", #condition);     \
			throw std::logic_error("Check failed, received NULL"); \
		}                                                              \
	} while (false)

#define CHECK_PTR(ptr) CAFF_CHECK(ptr != nullptr)

#define CHECK_POSITIVE(num) CAFF_CHECK(num > 0)
#define CHECK_ENUM(prefix, value) CAFF_CHECK(IS_VALID_ENUM_VALUE(prefix, value))
