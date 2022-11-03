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
  ss << " where age > ? AND name = ?";
  std::cout << "sql: [" << ss.str() << "]" <<std::endl;

  // 验证 sql
  ps->Prepare(ss.str());

  // 设置 where 条件 & 绑定
  long limit_age = 19;
  ps->SetInt(0, limit_age );
  std::string name = "张三";
  ps->SetString(1, name);
  // 绑定参数
  ps->BindParam();

  // 根据 "结果集元信息" 申请绑定结果集空间；
  auto meta = ps->ObtainMetaDataWithResBound();

  // 执行对应的 查询 user 表
  std::vector<User> users;
  ps->Execute<User>(users, meta);

  // 查询结果处理（打印）
  for (const auto user : users) {
    printf("## %d,%s,%s,%d ##\n", user.id, user.name.data(), user.email.data(), user.age);
  }

  // 关闭prepare statement
  ps->Close();

#if 0
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
  auto meta1 = ps1->ObtainMetaDataWithResBound();

  // 查询test表
  std::vector<Test> tests;
  ps1->Execute(tests, meta1);

  // debug
  for (const auto test : tests) {
    printf("## %s,%s,%s,%d ##\n", test.username.data(), test.email.data(), test.password, test.create_time);
  }

  // 关闭
  ps1->Close();
#endif

  // 释放链接
  pool.ReleaseConnection(conn);

  return 0;
}
