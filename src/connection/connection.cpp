/*
 * @Author: garin.yang garin.yang@outlook.com
 * @Date: 2023-04-15 21:27:28
 * @LastEditors: garin.yang garin.yang@outlook.com
 * @LastEditTime: 2023-04-15 23:06:45
 * @FilePath: /cppbatis-1/connection/connection.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
* desc: encapsulation for connection to mysql.
* author: garin.yang
* date: 2022-09-21.
 */

#include "connection/connection.h"

Connection::Connection(const std::string& host, unsigned port, const std::string& user, const std::string& passwd, const std::string& db) {
  // std::cout << ">>> construct connection" << std::endl;
  // init mysql
  mysql_ = mysql_init(nullptr);
  if (!mysql_) {
    fprintf(stderr, "mysql_init failed, error_no:%d, err_info:%s\n",
            mysql_errno(mysql_), mysql_error(mysql_));
    exit(0);  // 数据库异常中断
  }

  // connect
  auto res = mysql_real_connect(mysql_,
                              host.c_str(),
                              user.c_str(),
                              passwd.c_str(),
                              db.c_str(),
                              port,
                              nullptr,
                              0);
  if (!res) {
    fprintf(stderr, "mysql_real_connect failed, error_no:%u, err_info:%s\n",
            mysql_errno(mysql_), mysql_error(mysql_));
    exit(0);  // 数据库异常中断
  }

  // 设置字符集
  if (mysql_set_character_set(mysql_, "utf8")) {
    fprintf(stderr, "mysql_set_character_set failed, error:%s\n", mysql_error(mysql_));
  }
}

Connection::~Connection() {
  // std::cout << "Connection::~Connection" << std::endl;
  mysql_close(mysql_);
}


MYSQL* Connection::GetMysqlInstance() {
  return mysql_;
}

bool Connection::IsAlive() {
  return mysql_ping(mysql_) == 0;
}

