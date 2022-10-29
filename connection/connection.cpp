/*
* desc: encapsulation for connection to mysql.
* author: garin.yang
* date: 2022-09-21.
 */

#include "connection.h"

Connection::Connection() {

  // std::cout << ">>> construct connection" << std::endl;

  // init
  mysql_ = mysql_init(nullptr);

  // connect (配置化)
  mysql_ = mysql_real_connect(mysql_,
                              "127.0.0.1",
                              "root",
                              "root",
                              "cppbatis",
                              3306,
                              nullptr,
                              0);
  if (!mysql_) {
    fprintf(stderr, "mysql_real_connect failed, error:%s\n", mysql_error(mysql_));
  }

  // 设置字符集
  if (mysql_set_character_set(mysql_, "utf8")) {
    fprintf(stderr, "mysql_set_character_set failed, error:%s\n", mysql_error(mysql_));
  }
}


Connection::~Connection() {

  // std::cout << ">>> construct connection" << std::endl;

  if (mysql_) {
    mysql_close(mysql_);
  }
}


MYSQL* Connection::GetMysqlInstance() {
  return mysql_;
}

