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
#include <string>
#include <unistd.h>


int main() {
  {
    // 创建一个ps对象
    auto ps = std::make_unique<PrepareStatement>();

    // 1.拼装插入 sql
    std::stringstream ss_w;
    ss_w << "INSERT INTO " << "user(name,age,email,ttt,address)";
    ss_w << " VALUES(?, ?, ?, ?, ?)";

    // 验证 sql
    ps->Prepare(ss_w.str());

    // 设置 where 条件 & 绑定
    std::string name = "这是中文";
    ps->SetString(0, name);
    
    int32_t age = 22;
    ps->SetInt32(1, age);
    
    std::string email = "xxx@xxx.com";
    ps->SetString(2, email);

    std::string ttt = "hahahah";
    ps->SetString(3, ttt);

    std::string address = "北京市海淀区";
    ps->SetString(4, address);

    // 绑定参数
    ps->BindParam();

    // 更新
    if (!ps->Update()) {
      std::cout << "update failed." << std::endl;
      return -1;
    }

    // std::cout << "affacted: " << ps->GetAffactedRows() << std::endl;
  }

  {
    // 创建一个ps对象
    auto ps = std::make_unique<PrepareStatement>();

    // 2.拼装查询 sql
    std::stringstream ss;
    ss << "SELECT * FROM ";
    ss << " user ";
    ss << " where age in (?, ?)";

    // 验证 sql
    ps->Prepare(ss.str());

    // 设置 where 条件 & 绑定
    int32_t limit_age = 1998;
    ps->SetInt32(0, limit_age);
    //std::string name = "张三";
    //ps->SetString(1, name);
    int64_t age1 = 22;
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
      printf("#1# %d, %s, %s, %ld, %s, %s ##\n", user.id, user.name.data(), user.email.data(), user.age, user.ttt.data(), user.address);
    }
  }

  {
    // 3.拼装更新 sql
    auto ps1 = std::make_unique<PrepareStatement>();

    std::stringstream ss_u;
    ss_u << "UPDATE " << "user";
    ss_u << " SET ";
    ss_u << " name = ? ";
    ss_u << " WHERE id = ? ";

    // 验证 sql
    ps1->Prepare(ss_u.str());

    // 设置 where 条件 & 绑定
    std::string name_u = "张三1";
    ps1->SetString(0, name_u);
    
    int32_t id = 1;
    ps1->SetInt32(1, id);

    // 绑定参数
    ps1->BindParam();

    // 更新
    if (!ps1->Update()) {
      std::cout << "update failed." << std::endl;
      return -1;
    }

  // std::cout << "affacted: " << ps1->GetAffactedRows() << std::endl;
  }

  return 0;
}
