/*
 * desc: .
 * author: garin.yang
 * date: 2022/10/29
 */

#ifndef CPPBATIS_PREPARE_STMT_H
#define CPPBATIS_PREPARE_STMT_H

#include <iostream>
#include <memory>
#include <sstream>
#include <mysql/mysql.h>
#include <cstring>
#include "connection/connection.h"
#include "data_struct/user.h"
#include <vector>
#include <map>
#include <any>
#include <memory>
#include <string.h>
#include "reflection/easy_json.h"

class PrepareStatement {

public:

  /*
   * 默认构造函数
   * */
  PrepareStatement() = default;

  /*
   * 重载构造函数
   * */
  PrepareStatement(const std::shared_ptr<Connection> conn_ptr);

  /*
   * 析构函数
   * */
  ~PrepareStatement();

  /*
   * 预执行sql
   * */
  void Prepare(std::string sql);

  /*
   * 设置整形参数
   * */
  void SetInt32(int index, int32_t& value);

  /*
   * 设置长整形参数
   * */
  void SetInt64(int index, int64_t& value);

  /*
   * 设置字符串参数
   * */
  void SetString(int index, std::string& value);

  /*
     * 查询 & 更新
     * */
  // std::map<std::string, std::pair<int, std::any>>

  template <class T> void Execute(std::vector<T>& obj, std::map<std::string, std::pair<int, std::any>>& res) {

    if (!mysql_stmt_) {
      fprintf(stderr, " please init prepare statement firstly.\n");
      return;
    }

    // 1. 执行
    if (mysql_stmt_execute(mysql_stmt_))
    {
      fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
      fprintf(stderr, " %s\n", mysql_error(conn_->GetMysqlInstance()));
      return;
    }

    // 获取结果
    int status = 0;
    while (true)
    {
      // fetch each col
      status = mysql_stmt_fetch(mysql_stmt_);
      //fprintf(stdout, "mysql_stmt_fetch, status:%d\n", status);
      if (status == 1 || status == MYSQL_NO_DATA) {
        //fprintf(stdout, "### MYSQL_NO_DATA ###\n");
        break;
      }

      // 结果集拼装成json对象
      Json::Value jres;

      for (auto it = res.begin(); it != res.end(); ++it) {
        // 分拣处理不同类型
        if (MYSQL_TYPE_LONG == it->second.first) {
           //std::cout << it->first << ":" << *std::any_cast<int64_t *>(it->second.second)<< " ";
          jres[it->first] = int(*std::any_cast<int64_t*>(it->second.second));  // 精度丢失
          
          // 清空当前缓存值
          memset(std::any_cast<int64_t*>(it->second.second), 0, sizeof(int64_t));

        } else if (MYSQL_TYPE_VAR_STRING == it->second.first
                   or MYSQL_TYPE_DATETIME == it->second.first
                   or MYSQL_TYPE_BLOB == it->second.first) {
          // std::cout << it->first << ":" <<std::any_cast<std::shared_ptr<char>>(it->second.second) << " ";
           //std::cout << it->first << ":" <<std::any_cast<char*>(it->second.second) << " ";
          jres[it->first] = (std::string)std::any_cast<char*>(it->second.second);

          // 清空当前缓存值
          memset(std::any_cast<char*>(it->second.second), 0, strlen(std::any_cast<char*>(it->second.second)));

        } else if (MYSQL_TYPE_TIMESTAMP2 == it->second.first or MYSQL_TYPE_LONGLONG == it->second.first) {
           //std::cout << it->first << ":" << *std::any_cast<int64_t *>(it->second.second) <<std::endl;
          jres[it->first] = (int32_t)*std::any_cast<int64_t*>(it->second.second);

          // 清空当前缓存值
          memset(std::any_cast<int64_t*>(it->second.second), 0, sizeof(int64_t));
        } else if (MYSQL_TYPE_DOUBLE == it->second.first or MYSQL_TYPE_FLOAT == it->second.first) {
          //std::cout << it->first << ":" << *std::any_cast<double*>(it->second.second) << std::endl;
          jres[it->first] = (double)*std::any_cast<double*>(it->second.second);

          // 清空当前缓存值
          memset(std::any_cast<double*>(it->second.second), 0, sizeof(double));
        } else {
          std::cout << "[LOOK] unhandled type:" << it->second.first << std::endl;
        }
      }
      //std::cout << std::endl;

      /************** 通用类型 ***************/
      // 返回值类型：泛型T
      // 返回对象不确定： 反射
      T t;
      t.unmarshal(jres);

      // 添加到结果集
      obj.emplace_back(std::move(t));
    }
  }

  /*
   * 获取结果集元数据并bind对应的buffer
   * */
  std::map<std::string, std::pair<int, std::any>> ObtainMetaDataWithResBound();

  /*
     * 获取影响行数
     * */
  [[maybe_unused]] int GetAffactedRows();

  /*
   * 绑定参数
   * */
  void BindParam();

private:

  std::shared_ptr<Connection> conn_;  // 连接对象指针

  MYSQL_STMT* mysql_stmt_;            // ps对象指针

  MYSQL_RES * res_;                   // sql执行结果集指针

  MYSQL_BIND bind_param_[20];         // 绑定参数（当前默认最大支持20个，后续动态比较合理一些）
  MYSQL_BIND bind_result_[50];         // 绑定结果（当前默认最大支持50个，后续动态比较合理一些）
};

#endif // CPPBATIS_PREPARE_STMT_H
