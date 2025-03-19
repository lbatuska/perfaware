#include "../include/json.h"

#include <optional>

std::optional<json> json::json_parser::parse() {
  skip_whitespace();

  if (text[pos] == '{') return parse_object();
  if (text[pos] == '[') return parse_array();
  if (text[pos] == '"') return parse_string();
  if (std::isdigit(text[pos]) || text[pos] == '-') return parse_number();
  if (text.substr(pos, 4) == "true") {
    pos += 4;
    return json(true);
  }
  if (text.substr(pos, 5) == "false") {
    pos += 5;
    return json(false);
  }
  if (text.substr(pos, 4) == "null") {
    pos += 4;
    return json();
  }
  return std::nullopt;
}

std::optional<json> json::json_parser::parse_object() {
  pos++;  // Consume '{'
  skip_whitespace();
  std::map<std::string, json> obj;
  while (pos < text.size() && text[pos] != '}') {
    skip_whitespace();
    auto const maybekey = parse_string();
    if (!maybekey.has_value()) {
      return std::nullopt;
    }
    std::string key = std::move(maybekey.value().get<std::string>());
    skip_whitespace();

    if (text[pos] != ':') return std::nullopt;
    pos++;  // Consume ':'
    skip_whitespace();

    auto const maybevalue = parse();
    if (!maybevalue.has_value()) {
      obj[key] = nullptr;
    } else {
      obj[key] = json(std::move(maybevalue.value()));
    }

    skip_whitespace();
    if (text[pos] == ',') pos++;  // Consume ',' and continue
  }
  if (pos >= text.size() || text[pos] != '}') return std::nullopt;
  pos++;  // Consume '}'
  return json(obj);
}

std::optional<json> json::json_parser::parse_array() {
  pos++;  // Consume '['
  skip_whitespace();
  std::vector<json> arr;

  while (pos < text.size() && text[pos] != ']') {
    auto const res = parse();
    if (res.has_value()) {
      arr.push_back(std::move(res.value()));
    }
    skip_whitespace();
    if (text[pos] == ',') pos++;  // Consume ','
  }

  if (pos >= text.size() || text[pos] != ']') return std::nullopt;
  pos++;  // Consume ']'
  return json(arr);
}

std::optional<json> json::json_parser::parse_string() {
  if (text[pos] != '"') return std::nullopt;
  pos++;  // Consume '"'
  std::string result;
  bool escaped = false;
  while (pos < text.size()) {
    char c = text[pos++];
    if (escaped) {
      result += c;
      escaped = false;
      continue;
    }
    if (c == '\\') {
      escaped = true;
      result += c;
      continue;
    }
    if (c == '"') {
      pos--;
      break;
    }
    result += c;
  }
  if (pos >= text.size() || text[pos] != '"') return std::nullopt;
  pos++;  // Consume '"'
  return json(result);
}

std::optional<json> json::json_parser::parse_number() {
  size_t start = pos;
  while (pos < text.size() &&
         (std::isdigit(text[pos]) || text[pos] == '.' || text[pos] == '-' ||
          text[pos] == 'e' || text[pos] == 'E')) {
    pos++;
  }
  std::string num_str = text.substr(start, pos - start);

  if (num_str.find('.') != std::string::npos) {
    try {
      return json(std::stod(num_str));
    } catch (...) {
    }
  }
  try {
    return json(int64_t(std::stoll(num_str)));

  } catch (...) {
  }
  return std::nullopt;
}
