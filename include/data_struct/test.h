/*
 * desc: .
 * author: garin.yang
 * date: 2022/11/3
 */

#ifndef CPPBATIS_TEST_H
#define CPPBATIS_TEST_H
#include <iostream>
#include "reflection/easy_json.h"

struct Test {
  std::string username;
  std::string email;
  char password[128];
  int create_time;

  JSON_HELP(username, email, password, create_time);
};
#endif // CPPBATIS_TEST_H
