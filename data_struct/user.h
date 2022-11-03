/*
 * desc: .
 * author: garin.yang
 * date: 2022/10/29
 */

#ifndef CPPBATIS_USER_H
#define CPPBATIS_USER_H

#include "reflection/easy_json.h"

struct User{
  User():id(1988),name("garin"),email("garin@test.com"),age(35){}
  int id;
  std::string name;
  std::string email;
  int age;

  JSON_HELP(id, name, email, age);
};


#endif // CPPBATIS_USER_H
