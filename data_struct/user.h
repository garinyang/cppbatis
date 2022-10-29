/*
 * desc: .
 * author: garin.yang
 * date: 2022/10/29
 */

#ifndef CPPBATIS_USER_H
#define CPPBATIS_USER_H

struct User{
  User():id(1988),name("garin"),email("garin@test.com"),age(35){}
  int id;
  char name[16];
  char email[128];
  int age;
};


#endif // CPPBATIS_USER_H
