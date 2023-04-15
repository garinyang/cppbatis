/*
 * @Author: garin.yang garin.yang@outlook.com
 * @Date: 2023-04-15 21:27:28
 * @LastEditors: garin.yang garin.yang@outlook.com
 * @LastEditTime: 2023-04-15 22:49:11
 * @FilePath: /cppbatis-1/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
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
#include <unistd.h>


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
  ss << " where age in (?, ?)";
  std::cout << "sql: [" << ss.str() << "]" <<std::endl;

  // 验证 sql
  ps->Prepare(ss.str());

  // 设置 where 条件 & 绑定
  int32_t limit_age = 1998;
  ps->SetInt32(0, limit_age);
  //std::string name = "张三";
  //ps->SetString(1, name);
  int64_t age1 = 2120202022;
  ps->SetInt64(1, age1);
  // 绑定参数
  ps->BindParam();

  // 根据 "结果集元信息" 申请绑定结果集空间；
  auto meta = ps->ObtainMetaDataWithResBound();

  // 执行对应的 查询 user 表
  std::vector<User> users;
  ps->Execute(users, meta);

  // 查询结果处理（打印）
  for (const auto user : users) {
    printf("#1# %d, %s, %s, %lld, %s ##\n", user.id, user.name.data(), user.email.data(), user.age, user.ttt.data());
  }

  // sleep 10秒
  // sleep 头文件是 unistd.h  
  sleep(10);

  std::vector<User> users1;
  ps->Execute(users1, meta);

  // 查询结果处理（打印）
  for (const auto user : users1) {
    printf("#2# %d, %s, %s, %lld, %s ##\n", user.id, user.name.data(), user.email.data(), user.age, user.ttt.data());


    // easy_json 目前对于中文支持有点问题
//    Json::FastWriter fw;
//    Json::Value js_user;
//    user.marshal(js_user);
//    auto u = fw.write(js_user);
//    printf("-> %s", u.c_str());
  }

  // 释放链接
  pool.ReleaseConnection(conn);

  return 0;
}
