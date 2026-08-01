#include "NativePackageAPI.hpp"

#include <chrono>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <thread>

namespace {

void err(ExprPackageStringView *e, const char *message) {
  if (e)
    *e = {message, std::strlen(message)};
}

template <typename OutputDuration, typename InputDuration>
bool checkedCount(InputDuration input, int64_t &output,
                  ExprPackageStringView *e, const char *functionName) {
  using FloatingDuration =
      std::chrono::duration<long double, typename OutputDuration::period>;
  const long double value = FloatingDuration(input).count();
  const long double minimum =
      static_cast<long double>(std::numeric_limits<int64_t>::min());
  const long double maximum =
      static_cast<long double>(std::numeric_limits<int64_t>::max());
  if (!std::isfinite(value) || value < minimum || value > maximum) {
    static thread_local std::string message;
    message = std::string(functionName) + " result is outside the i64 range";
    if (e)
      *e = {message.c_str(), message.size()};
    return false;
  }
  output = std::chrono::duration_cast<OutputDuration>(input).count();
  return true;
}

template <typename OutputDuration, typename Clock>
bool clockValue(const ExprPackageValue *args, size_t count,
                ExprPackageValue *result, ExprPackageStringView *error,
                const char *functionName) {
  (void)args;
  if (count != 0) {
    static thread_local std::string message;
    message = std::string(functionName) + " expects no arguments";
    if (error)
      *error = {message.c_str(), message.size()};
    return false;
  }
  int64_t value = 0;
  if (!checkedCount<OutputDuration>(Clock::now().time_since_epoch(), value,
                                    error, functionName))
    return false;
  result->kind = EXPR_PACKAGE_VALUE_I64;
  result->as.i64_value = value;
  return true;
}

bool monotonicMillis(const ExprHostApi *, const ExprPackageValue *args,
                     size_t count, ExprPackageValue *result,
                     ExprPackageStringView *error) {
  return clockValue<std::chrono::milliseconds, std::chrono::steady_clock>(
      args, count, result, error, "monotonicMillis");
}

bool monotonicNanos(const ExprHostApi *, const ExprPackageValue *args,
                    size_t count, ExprPackageValue *result,
                    ExprPackageStringView *error) {
  return clockValue<std::chrono::nanoseconds, std::chrono::steady_clock>(
      args, count, result, error, "monotonicNanos");
}

bool unixMillis(const ExprHostApi *, const ExprPackageValue *args, size_t count,
                ExprPackageValue *result, ExprPackageStringView *error) {
  return clockValue<std::chrono::milliseconds, std::chrono::system_clock>(
      args, count, result, error, "unixMillis");
}

bool unixSeconds(const ExprHostApi *, const ExprPackageValue *args,
                 size_t count, ExprPackageValue *result,
                 ExprPackageStringView *error) {
  return clockValue<std::chrono::seconds, std::chrono::system_clock>(
      args, count, result, error, "unixSeconds");
}

template <typename Duration>
bool sleepFor(const ExprPackageValue *args, size_t count,
              ExprPackageValue *result, ExprPackageStringView *error,
              const char *functionName) {
  if (count != 1 || !args || args[0].kind != EXPR_PACKAGE_VALUE_I64 ||
      args[0].as.i64_value < 0) {
    static thread_local std::string message;
    message = std::string(functionName) + " expects one non-negative i64";
    if (error)
      *error = {message.c_str(), message.size()};
    return false;
  }
  const int64_t value = args[0].as.i64_value;
  using Representation = typename Duration::rep;
  if (static_cast<long double>(value) >
      static_cast<long double>(std::numeric_limits<Representation>::max())) {
    err(error, "sleep duration is outside the platform-supported range");
    return false;
  }
  std::this_thread::sleep_for(Duration(static_cast<Representation>(value)));
  result->kind = EXPR_PACKAGE_VALUE_NULL;
  return true;
}

bool sleepMillis(const ExprHostApi *, const ExprPackageValue *args,
                 size_t count, ExprPackageValue *result,
                 ExprPackageStringView *error) {
  return sleepFor<std::chrono::milliseconds>(args, count, result, error,
                                              "sleepMillis");
}

bool sleepSeconds(const ExprHostApi *, const ExprPackageValue *args,
                  size_t count, ExprPackageValue *result,
                  ExprPackageStringView *error) {
  return sleepFor<std::chrono::seconds>(args, count, result, error,
                                        "sleepSeconds");
}

constexpr ExprPackageFunctionExport functions[] = {
    {"monotonicMillis", "fn() -> i64", 0, monotonicMillis},
    {"monotonicNanos", "fn() -> i64", 0, monotonicNanos},
    {"unixMillis", "fn() -> i64", 0, unixMillis},
    {"unixSeconds", "fn() -> i64", 0, unixSeconds},
    {"sleepMillis", "fn(i64) -> void", 1, sleepMillis},
    {"sleepSeconds", "fn(i64) -> void", 1, sleepSeconds},
};

constexpr ExprPackageRegistration registration = {
    3, "github", "time", functions, 6, nullptr, 0};

} // namespace

extern "C" const ExprPackageRegistration *exprRegisterPackage() {
  return &registration;
}
