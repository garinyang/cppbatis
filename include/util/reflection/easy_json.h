/*
 * desc: .
 * author: garin.yang
 * date: 2022/11/3
 */

#ifndef CPPBATIS_EASY_JSON_H
#define CPPBATIS_EASY_JSON_H
#include <assert.h>
// #include <json/json.h>
#include "json/json.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Json {
using namespace std;

/*======  Unmarshal实现  =======*/

// 检测一个类 是否含有非静态非重载的unmarshal方法
template <typename T>
struct TestUnmarshalFunc {
  template <typename TT>
  static char func(decltype(&TT::unmarshal));
  template <typename TT>
  static int func(...);
  const static bool has      = (sizeof(func<T>(NULL)) == sizeof(char));
  const static bool has_enum = std::is_enum<T>::value;
};

//如果对象自身含有 unmarshal 方法，则调用对象的unmarshal
template <typename T, typename enable_if<TestUnmarshalFunc<T>::has, int>::type = 0>
inline bool Unmarshal(T& obj, nlohmann::json& root) {
  return obj.unmarshal(root);
}

//如果对象为enum类型，则转为对应的枚举类型
template <typename T, typename enable_if<TestUnmarshalFunc<T>::has_enum, int>::type = 0>
inline bool Unmarshal(T& obj, nlohmann::json& root) {
  int temp = root.get<int>();
  obj      = static_cast<T>(temp);
  return true;
}

//特例化基本类型 int long bool float double string char
// template <int = 0>
// inline bool Unmarshal(char& obj, nlohmann::json& root) {
//   if (!root.is_string())
//     return false;
//   auto tmp = root.get<std::string>();
//   assert(tmp.size() == 1);
//   obj = tmp[0];
//   return true;
// }

template <int = 0>
inline bool Unmarshal(int8_t& obj, nlohmann::json& root) {
  if (!root.is_number_integer())
    return false;
  obj = root.get<int8_t>();
  return true;
}

template <int = 0>
inline bool Unmarshal(uint8_t& obj, nlohmann::json& root) {
  if (!root.is_number_unsigned())
    return false;
  obj = root.get<uint8_t>();
  return true;
}

template <int = 0>
inline bool Unmarshal(int16_t& obj, nlohmann::json& root) {
  if (!root.is_number_integer())
    return false;
  obj = root.get<int16_t>();
  return true;
}

template <int = 0>
inline bool Unmarshal(uint16_t& obj, nlohmann::json& root) {
  if (!root.is_number_unsigned())
    return false;
  obj = root.get<uint16_t>();
  return true;
}

template <int = 0>
inline bool Unmarshal(int32_t& obj, nlohmann::json& root) {
  if (!root.is_number_integer())
    return false;
  obj = root.get<int32_t>();
  return true;
}

template <int = 0>
inline bool Unmarshal(uint32_t& obj, nlohmann::json& root) {
  if (!root.is_number_unsigned())
    return false;
  obj = root.get<uint32_t>();
  return true;
}

template <int = 0>
inline bool Unmarshal(int64_t& obj, nlohmann::json& root) {
  if (!root.is_number_integer())
    return false;
  obj = root.get<int64_t>();
  return true;
}

template <int = 0>
inline bool Unmarshal(uint64_t& obj, nlohmann::json& root) {
  if (!root.is_number_unsigned())
    return false;
  obj = root.get<uint64_t>();
  return true;
}

// template <int = 0>
// inline bool Unmarshal(long& obj, nlohmann::json& root) {
//   if (!root.is_number_integer())
//     return false;
//   obj = root.get<int64_t>();
//   return true;
// }

template <int = 0>
inline bool Unmarshal(bool& obj, nlohmann::json& root) {
  if (!root.is_boolean())
    return false;
  obj = root.get<bool>();
  return true;
}

template <int = 0>
inline bool Unmarshal(float& obj, nlohmann::json& root) {
  if (root.is_number_float())
    obj = root.get<float>();
  else if (root.is_number_integer())
    obj = static_cast<float>(root.get<int32_t>());
  else return false;
  return true;
}

template <int = 0>
inline bool Unmarshal(double& obj, nlohmann::json& root) {
  if (root.is_number_float())
    obj = root.get<float>();
  else if (root.is_number_integer())
    obj = static_cast<double>(root.get<int32_t>());
  else return false;
  return true;
}

template <int = 0>
inline bool Unmarshal(string& obj, nlohmann::json& root) {
  if (!root.is_string())
    return false;
  obj = root.get<string>();
  return true;
}

template <int = 0>
inline bool Unmarshal(char* obj, nlohmann::json& root) {
  if (!root.is_string())
    return false;
  strcpy(obj, root.get<std::string>().c_str());
  return true;
}
//特例化stl容器或其他不含unmarshal方法的对象
template <typename T>
bool Unmarshal(vector<T>&, nlohmann::json&);  //在模板相互循环调用对方时,必须先声明,使所有类型的 模板可见!
template <typename T>
bool Unmarshal(map<string, T>&, nlohmann::json&);
template <typename T>
bool Unmarshal(map<int, T>& obj, nlohmann::json& root);
template <typename T>
bool Unmarshal(map<long, T>& obj, nlohmann::json& root);

template <typename T>
bool Unmarshal(optional<T>& obj, nlohmann::json& root);  // optional 类型, 表示该字段可以为空

// vector
template <typename T>
bool Unmarshal(vector<T>& obj, nlohmann::json& root) {
#if 0
  if (!root.isArray())
    return false;
  obj.clear();
  bool ret = true;
  for (int i = 0; i < root.size(); ++i) {
    T tmp;  //类型T一定要含有默认构造器！
    if (!Unmarshal(tmp, root[i]))
      ret = false;
    obj.push_back(tmp);
  }
#else
  if (!root.is_array())
   return false;
  obj.clear();
  bool ret = true;
  for (int i = 0; i < root.size(); ++i) {
    T tmp;  //类型T一定要含有默认构造器！
    if (!Unmarshal(tmp, root[i]))
      ret = false;
    obj.push_back(tmp);
  }
#endif
  return ret;
}

// map key:string
template <typename T>
bool Unmarshal(map<string, T>& obj, nlohmann::json& root) {
#if 0
  if (!root.isObject())
    return false;
  obj.clear();
  auto mems = root.getMemberNames();
  bool ret  = true;
  for (auto it = mems.begin(); it != mems.end(); ++it) {
    if (!Unmarshal(obj[*it], root[*it]))
      ret = false;
  }
#else
  if (!root.is_object())
    return false;
  obj.clear();
  // auto mems = root.getMemberNames();
  bool ret  = true;
  for (auto it = root.begin(); it != root.end(); ++it) {
    if (!Unmarshal(obj[*it], root[*it]))
      ret = false;
  }
#endif
  return ret;
}

// map key:int
template <typename T>
bool Unmarshal(map<int, T>& obj, nlohmann::json& root) {
#if 0
  if (!root.isObject())
    return false;
  obj.clear();
  auto mems = root.getMemberNames();
  bool ret  = true;
  for (auto it = mems.begin(); it != mems.end(); ++it) {
    if (!Unmarshal(obj[atoi(it->c_str())], root[*it]))
      ret = false;
  }
#else
  if (!root.is_object())
    return false;
  obj.clear();
  //auto mems = root.getMemberNames();
  bool ret  = true;
  for (auto it = root.begin(); it != root.end(); ++it) {
    if (!Unmarshal(obj[atoi(it.key().c_str())], root[*it]))
      ret = false;
  }
#endif
  return ret;
}

// map key:long
template <typename T>
bool Unmarshal(map<long, T>& obj, nlohmann::json& root) {
  // if (!root.isObject())
  if (!root.is_object())
    return false;
  obj.clear();
  #if 0
  auto mems = root.getMemberNames();
  bool ret  = true;
  for (auto it = mems.begin(); it != mems.end(); ++it) {
    if (!Unmarshal(obj[atol(it->c_str())], root[*it]))
      ret = false;
  }
  #else
    bool ret  = true;
    for (auto it = root.begin(); it != root.end(); ++it) {
      if (!Unmarshal(obj[atol(it.key().c_str())], root[*it]))
      ret = false;
    }  
  #endif
  return ret;
}

// optional
template <typename T>
bool Unmarshal(optional<T>& obj, nlohmann::json& root) {
  T tmp;  // 类型T一定要含有默认构造器！
  if (Unmarshal(tmp, root)) {
    obj = tmp;
  }
  else {
    obj = std::nullopt;
  }
  return true;
}

/*======  Marshal实现  =======*/

// 检测一个类 是否含有非静态非重载的marshal方法
template <typename T>
struct TestMarshalFunc {
  template <typename TT>
  static char func(decltype(&TT::marshal));
  template <typename TT>
  static int func(...);
  const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
};

template <typename T, typename enable_if<TestMarshalFunc<T>::has, int>::type = 0>
inline void Marshal(const T& obj, nlohmann::json& root) {
  obj.marshal(root);
}
template <typename T, typename enable_if<!TestMarshalFunc<T>::has, int>::type = 0>
inline void Marshal(const T& obj, nlohmann::json& root) {
  root = obj;
}

//特例化jsoncpp 不支持的基本类型
template <int = 0>
inline void Marshal(long obj, nlohmann::json& root) {
  // root = Int64(obj);
   root = int64_t(obj);
}

//特例化jsoncpp 不支持的基本类型
template <int = 0>
inline void Marshal(uint64_t obj, nlohmann::json& root) {
  // root = UInt64(obj);
  root = uint64_t(obj);
}

//特例化容器或没有实现marshal函数的类 vector map
template <typename T>
void Marshal(const vector<T>&, nlohmann::json&);
template <typename T>
void Marshal(const map<string, T>&, nlohmann::json&);
template <typename T>
void Marshal(const map<int, T>& obj, nlohmann::json& root);
template <typename T>
void Marshal(const map<long, T>& obj, nlohmann::json& root);

template <typename T>
void Marshal(const optional<T>& obj, nlohmann::json& root);  // optional 类型, 表示该字段可以为空

// vector
template <typename T>
void Marshal(const vector<T>& obj, nlohmann::json& root) {
  for (int i = 0; i < obj.size(); ++i) {
    Marshal(obj[i], root[i]);
  }
}

// map key:string
template <typename T>
void Marshal(const map<string, T>& obj, nlohmann::json& root) {
  for (auto it = obj.begin(); it != obj.end(); ++it) {
    Marshal(it->second, root[it->first]);
  }
}

// map key:int
template <typename T>
void Marshal(const map<int, T>& obj, nlohmann::json& root) {
  char num_buf[15] = {0};
  for (auto it = obj.begin(); it != obj.end(); ++it) {
    snprintf(num_buf, 15, "%d", it->first);
    Marshal(it->second, root[num_buf]);
  }
}

// map key:long
template <typename T>
void Marshal(const map<long, T>& obj, nlohmann::json& root) {
  char num_buf[25] = {0};
  for (auto it = obj.begin(); it != obj.end(); ++it) {
    snprintf(num_buf, 25, "%ld", it->first);
    Marshal(it->second, root[num_buf]);
  }
}

// map key:shared_ptr
template <typename T>
void Marshal(std::shared_ptr<T> obj, nlohmann::json& root) {
  Marshal(*obj.get(), root);
}

// optional 类型, 表示该字段可以为空
template <typename T>
void Marshal(const optional<T>& obj, nlohmann::json& root) {
  if (obj) {
    Marshal(obj.value(), root);
  }
}

/*========  string 版本  ===========*/
template <typename T>
void Marshal(const T& obj, string& s) {
  nlohmann::json root;
  //Json::FastWriter writer;
  Marshal(obj, root);
  if (root.is_null())
    s = "";
  else
    //s = writer.write(root);
    s = root.dump();
}

template <typename T>
bool Unmarshal(T& obj, const string& s) {
  //Json::Reader reader;
  nlohmann::json root;
  root = nlohmann::json::parse(s); 
  // if (!reader.parse(s, root))
  //   return false;
  return Unmarshal(obj, root);
}
}  // namespace Json

// 一些有用的宏,小写变量命名的宏只限本文件内使用

// 将任意宏完全展开为字符串,调试用
#define TO_STRING(...) __to_string__(__VA_ARGS__)
#define __to_string__(...) #__VA_ARGS__

// 统计可变参数个数的宏
#define COUNT(...) __count__(0, ##__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __count__(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, \
                  ...)                                                                                              \
    N

// 将两个宏展开后,连接在一起
#define MACRO_CAT(a, b) __macro_cat__(a, b)
#define __macro_cat__(a, b) a##b

// 宏展开实现伪循环
#define __func_1(func, member) func(member);
#define __func_2(func, member, ...) __func_1(func, member) __func_1(func, __VA_ARGS__)
#define __func_3(func, member, ...) __func_1(func, member) __func_2(func, __VA_ARGS__)
#define __func_4(func, member, ...) __func_1(func, member) __func_3(func, __VA_ARGS__)
#define __func_5(func, member, ...) __func_1(func, member) __func_4(func, __VA_ARGS__)
#define __func_6(func, member, ...) __func_1(func, member) __func_5(func, __VA_ARGS__)
#define __func_7(func, member, ...) __func_1(func, member) __func_6(func, __VA_ARGS__)
#define __func_8(func, member, ...) __func_1(func, member) __func_7(func, __VA_ARGS__)
#define __func_9(func, member, ...) __func_1(func, member) __func_8(func, __VA_ARGS__)
#define __func_10(func, member, ...) __func_1(func, member) __func_9(func, __VA_ARGS__)
#define __func_11(func, member, ...) __func_1(func, member) __func_10(func, __VA_ARGS__)
#define __func_12(func, member, ...) __func_1(func, member) __func_11(func, __VA_ARGS__)
#define __func_13(func, member, ...) __func_1(func, member) __func_12(func, __VA_ARGS__)
#define __func_14(func, member, ...) __func_1(func, member) __func_13(func, __VA_ARGS__)
#define __func_15(func, member, ...) __func_1(func, member) __func_14(func, __VA_ARGS__)
#define __func_16(func, member, ...) __func_1(func, member) __func_15(func, __VA_ARGS__)
#define __func_17(func, member, ...) __func_1(func, member) __func_16(func, __VA_ARGS__)
#define __func_18(func, member, ...) __func_1(func, member) __func_17(func, __VA_ARGS__)
#define __func_19(func, member, ...) __func_1(func, member) __func_18(func, __VA_ARGS__)
#define __func_20(func, member, ...) __func_1(func, member) __func_19(func, __VA_ARGS__)

// 传入一个函数func和一个list，把func用在list的每个元素上
#define FOR_EACH(func, ...) MACRO_CAT(__func_, COUNT(__VA_ARGS__))(func, __VA_ARGS__)

//在一个对象内部生成 unmarshal 函数.
//尽力解析每个field,但不保证全部正确,未被正确解析的field保持原有值不变. 所有field 均被正确解析时返回true
#define UNMARSHAL_OBJ(...)                                  \
    bool unmarshal(nlohmann::json& root) {               \
        bool ret = true;                                    \
        FOR_EACH(__unmarshal_obj_each_field__, __VA_ARGS__) \
        return ret;                                         \
    }
#define __unmarshal_obj_each_field__(field)      \
    if (!Json::Unmarshal(field, root[#field])) { \
        ret = false;                             \
    }

//在一个对象内部生成 marshal 函数
#define MARSHAL_OBJ(...) \
    void marshal(nlohmann::json& root) const { FOR_EACH(__marshal_obj_each_field__, __VA_ARGS__) }
#define __marshal_obj_each_field__(field) Json::Marshal(field, root[#field]);

//在一个对象内部生成getTableName 函数,用于生成表名
#define GET_TABLE_NAME(table_name) \
    static std::string getTableName() { return #table_name; }

//在一个对象内部生成getQueryBaseSql 函数,用于生成查询语句的基本部分
#define GET_QUERY_BASE_SQL(...) \
    static std::string getQueryBaseSql() { return std::string("SELECT ") + #__VA_ARGS__ + " FROM " + getTableName() + std::string(" "); }

//让一个对象可以被 unmarshal和marshal
#define JSON_HELP(table_name, ...)         \
    UNMARSHAL_OBJ(__VA_ARGS__) \
    MARSHAL_OBJ(__VA_ARGS__)   \
    GET_TABLE_NAME(table_name) \
    GET_QUERY_BASE_SQL(__VA_ARGS__)

#endif // CPPBATIS_EASY_JSON_H
