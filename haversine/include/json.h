#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

class json;

using json_value =
    std::variant<std::nullptr_t, bool, int64_t, double, std::string,
                 std::vector<json>, std::map<std::string, json>>;

class json {
 private:
  json_value value;

 public:
  json& operator[](char const* key) {
    if (!std::holds_alternative<std::map<std::string, json>>(value)) {
      value = std::map<std::string, json>();
    }
    return std::get<std::map<std::string, json>>(value)[key];
  }

  json& operator[](std::string const& key) {
    if (!std::holds_alternative<std::map<std::string, json>>(value)) {
      value = std::map<std::string, json>();
    }
    return std::get<std::map<std::string, json>>(value)[key];
  }

  template <typename T>
  json& operator=(T&& value) {
    this->value = std::forward<T>(value);
    return *this;
  }

  json() : value(nullptr) {}
  json(bool b) : value(b) {}
  json(int64_t i) : value(i) {}
  json(double d) : value(d) {}
  json(const std::string& s) : value(s) {}
  json(std::vector<json> arr) : value(std::move(arr)) {}
  json(std::map<std::string, json> obj) : value(std::move(obj)) {}

  static std::optional<json> parse(std::string json_string) {
    auto jp = json_parser(std::move(json_string));
    auto val = jp.parse();
    return val;
  }

  template <typename T>
  T get() const {
    if (std::holds_alternative<T>(value)) {
      return std::get<T>(value);
    }
    return T();
  }

  friend std::ostream& operator<<(std::ostream& os, const json& j) {
    if (std::holds_alternative<std::nullptr_t>(j.value)) {
      os << "null";
      return os;
    } else if (std::holds_alternative<int64_t>(j.value)) {
      os << std::get<int64_t>(j.value);
      return os;
    } else if (std::holds_alternative<bool>(j.value)) {
      os << (std::get<bool>(j.value) ? "true" : "false");
      return os;
    } else if (std::holds_alternative<double>(j.value)) {
      os << std::get<double>(j.value);
      return os;
    } else if (std::holds_alternative<std::string>(j.value)) {
      os << "\"" << std::get<std::string>(j.value) << "\"";
      return os;
    } else if (std::holds_alternative<std::map<std::string, json>>(j.value)) {
      os << "{\n";
      auto const& obj = std::get<std::map<std::string, json>>(j.value);
      for (auto it = obj.begin(); it != obj.end(); ++it) {
        os << "  \"" << it->first << "\": " << it->second;
        if (std::next(it) != obj.end()) os << ",";
        os << "\n";
      }
      os << "}";
    } else if (std::holds_alternative<std::vector<json>>(j.value)) {
      os << "[\n";
      const auto& arr = std::get<std::vector<json>>(j.value);
      for (size_t i = 0; i < arr.size(); ++i) {
        os << "  " << arr[i];
        if (i + 1 < arr.size()) os << ",";
        os << "\n";
      }
      os << "]";
    }
    return os;
  }

  operator int64_t() { return std::get<int64_t>(value); }
  bool operator==(int i) const {
    return std::holds_alternative<int64_t>(value) &&
           std::get<int64_t>(value) == i;
  }

  operator bool() { return std::get<bool>(value); }
  bool operator==(bool b) const {
    return std::holds_alternative<bool>(value) && std::get<bool>(value) == b;
  }

 private:
  class json_parser {
   private:
    ;
    std::string text;
    size_t pos = 0;

   public:
    json_parser(std::string json_text) : text(std::move(json_text)) {}
    std::optional<json> parse();

   private:
    inline void skip_whitespace() {
      while (pos < text.size() && std::isspace(text[pos])) pos++;
    };
    std::optional<json> parse_object();  // {}
    std::optional<json> parse_array();   // []
    std::optional<json> parse_string();  // '"'
    std::optional<json> parse_number();
  };
};
