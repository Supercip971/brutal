#pragma once

#include <brutal/base/any.h>
#include <brutal/debug/log.h>
#include <brutal/debug/source_location.h>

/* --- Unlocked log functions ----------------------------------------------- */

void log_unlock_impl(LogLevel level, SourceLocation location, Str fmt, AnyVa args);

#define log_unlock(fmt, ...) \
    log_unlock_impl(LOG_DEFAULT, source_location_current, str$(fmt), any_va$(__VA_ARGS__))
