#ifndef SRC_NODE_URL_PATTERN_H_
#define SRC_NODE_URL_PATTERN_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "ada.h"
#include "base_object.h"
#include "util.h"

#include <v8.h>

#include <optional>
#include <string_view>

namespace node::url_pattern {

#define URL_PATTERN_COMPONENTS(V)                                              \
  V(Protocol, protocol)                                                        \
  V(Username, username)                                                        \
  V(Password, password)                                                        \
  V(Hostname, hostname)                                                        \
  V(Port, port)                                                                \
  V(Pathname, pathname)                                                        \
  V(Search, search)                                                            \
  V(Hash, hash)

// By default, ada::url_pattern doesn't ship with any regex library.
// Ada has a std::regex implementation, but it is considered unsafe and does
// not have a fully compliant ecmascript syntax support. Therefore, Ada
// supports passing custom regex provider that conforms to the following
// class and function structure. For more information, please look into
// url_pattern_regex.h inside github.com/ada-url/ada.
class URLPatternRegexProvider {
 public:
  using regex_type = v8::Global<v8::RegExp>;
  static std::optional<regex_type> create_instance(std::string_view pattern,
                                                   bool ignore_case);
  static std::optional<std::vector<std::optional<std::string>>> regex_search(
      std::string_view input, const regex_type& pattern);
  static bool regex_match(std::string_view input, const regex_type& pattern);
};

class URLPattern : public BaseObject {
 public:
  URLPattern(Environment* env,
             v8::Local<v8::Object> object,
             ada::url_pattern<URLPatternRegexProvider>&& url_pattern);

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

  // V8 APIs
  // - Functions
  static void Exec(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Test(const v8::FunctionCallbackInfo<v8::Value>& info);
  // - Component Getters
#define URL_PATTERN_COMPONENT_GETTERS(name, _)                                 \
  static void name(const v8::FunctionCallbackInfo<v8::Value>& info);
  URL_PATTERN_COMPONENTS(URL_PATTERN_COMPONENT_GETTERS)
#undef URL_PATTERN_COMPONENT_GETTERS
  // - Has Regexp Groups
  static void HasRegexpGroups(const v8::FunctionCallbackInfo<v8::Value>& info);

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(URLPattern)
  SET_SELF_SIZE(URLPattern)

  class URLPatternInit {
   public:
    static std::optional<ada::url_pattern_init> FromJsObject(
        Environment* env, v8::Local<v8::Object> obj);
    static v8::MaybeLocal<v8::Value> ToJsObject(
        Environment* env, const ada::url_pattern_init& init);
  };

  class URLPatternOptions {
   public:
    static std::optional<ada::url_pattern_options> FromJsObject(
        Environment* env, v8::Local<v8::Object> obj);
  };

  class URLPatternResult {
   public:
    static v8::MaybeLocal<v8::Value> ToJSValue(
        Environment* env, const ada::url_pattern_result& result);
  };

  class URLPatternComponentResult {
   public:
    static v8::MaybeLocal<v8::Object> ToJSObject(
        Environment* env, const ada::url_pattern_component_result& result);
  };

 private:
  ada::url_pattern<URLPatternRegexProvider> url_pattern_;
  // Getter methods
#define URL_PATTERN_COMPONENT_GETTERS(name, _) v8::MaybeLocal<v8::Value> name();
  URL_PATTERN_COMPONENTS(URL_PATTERN_COMPONENT_GETTERS)
#undef URL_PATTERN_COMPONENT_GETTERS
  bool HasRegExpGroups() const;
  // Public API
  v8::MaybeLocal<v8::Value> Exec(
      Environment* env,
      const ada::url_pattern_input& input,
      std::optional<std::string_view>& baseURL);  // NOLINT (runtime/references)
  bool Test(
      Environment* env,
      const ada::url_pattern_input& input,
      std::optional<std::string_view>& baseURL);  // NOLINT (runtime/references)

#define URL_PATTERN_CACHED_VALUES(_, name) v8::Global<v8::Value> name;
  URL_PATTERN_COMPONENTS(URL_PATTERN_CACHED_VALUES)
#undef URL_PATTERN_CACHED_VALUES
};

}  // namespace node::url_pattern

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
#endif  // SRC_NODE_URL_PATTERN_H_
