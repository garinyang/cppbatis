/*
 * desc: 测试入口函数.
 * author: garin.yang
 * date: 2022/10/29
 */

#include <iostream>
#include "connection/singleton_base.h"
#include "connection/singleton_base.h"
#include "connection/connection_pool.h"
#include "connection/connection.h"
#include "prepare_stmt/prepare_stmt.h"
#include "data_struct/user.h"
#include <mysql/mysql.h>
#include <memory>
#include <string.h>
#include <sstream>

int main() {

  // std::cout << "Hello, World!" << std::endl;
  //     auto conn = SingletonBase<ConnectPool>::ObtainInstance();
  //     std::cout << "max connection: " << conn.GetMaxConnNum() <<std::endl;
  //  1. 拼装 sql
  std::stringstream ss;
  ss << "SELECT * FROM ";
  ss << " user ";
  ss << " WHERE age >= ?";

  auto pool = SingletonBase<ConnectPool>::ObtainInstance();
  auto conn = pool.GetConnection();

  auto ps = std::make_unique<PrepareStatement>(conn);
  ps->Prepare(ss.str());

  auto meta = ps->GetMetaData();

  std::cout << "meta size:" << meta.size() << std::endl;

  //User user;
  //ps->BindResult(user);

  long limit = 10;
  ps->SetInt(0, limit);

  std::vector<User> users;
  ps->Query(users, meta);

  // printf("handle current row here, %d,%s,%s,%d\n", user.id, user.name, user.email, user.age);

  ps->Close();

  // 释放链接
  pool.ReleaseConnection(conn);

  // debug
  for (const auto user : users) {
    printf("## %d,%s,%s,%d ##\n", user.id, user.name, user.email, user.age);
  }

  return 0;
}
