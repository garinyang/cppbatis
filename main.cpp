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
#include "data_struct/test.h"
#include <mysql/mysql.h>
#include <memory>
#include <string.h>
#include <sstream>

int main() {

  // std::cout << "Hello, World!" << std::endl;
  //     auto conn = SingletonBase<ConnectPool>::ObtainInstance();
  //     std::cout << "max connection: " << conn.GetMaxConnNum() <<std::endl;

  // 获取连接池对象
  auto pool = SingletonBase<ConnectPool>::ObtainInstance();

  // 获取mysql连接对象
  auto conn = pool.GetConnection();

  // 创建一个ps对象
  auto ps = std::make_unique<PrepareStatement>(conn);

  // 拼装 sql
  std::stringstream ss;
  ss << "SELECT * FROM ";
  ss << " user ";
  ss << " where age > ?";
  std::cout << "sql1: " << ss.str() << std::endl;

  // 验证 sql
  ps->Prepare(ss.str());

  // 获取结果集元信息 并 绑定 结果集
  auto meta = ps->GetMetaData();

  // 设置 where 条件
  long limit = 0;
  ps->SetInt(0, limit);

  // 查询 user 表
  std::vector<User> users;
  ps->Query(users, meta);

  // debug
  for (const auto user : users) {
    printf("## %d,%s,%s,%d ##\n", user.id, user.name.data(), user.email.data(), user.age);
  }

  ps->Close();


  // new
  auto ps1 = std::make_unique<PrepareStatement>(conn);

  // 拼装 sql
  ss.clear();
  ss.str("");
  ss << "SELECT * FROM test ";
  std::cout << "sql2: " << ss.str() << std::endl;
  // 验证 sql
  ps1->Prepare(ss.str());

  // 获取结果集元信息 并 绑定 结果集
  auto meta1 = ps1->GetMetaData();

  // 查询test表
  std::vector<Test> tests;
  ps1->Query(tests, meta1);

  // debug
  for (const auto test : tests) {
    printf("## %s,%s,%s,%d ##\n", test.username.data(), test.email.data(), test.password, test.create_time);
  }

  // 关闭
  ps1->Close();

  // 释放链接
  pool.ReleaseConnection(conn);



  return 0;
}
