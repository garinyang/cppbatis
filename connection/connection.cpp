/*
* desc: encapsulation for connection to mysql.
* author: garin.yang
* date: 2022-09-21.
 */

#include "connection.h"

Connection::Connection() {

  // std::cout << ">>> construct connection" << std::endl;

  // init
  mysql_init(&mysql_);
  auto res = mysql_real_connect(&mysql_,
                              "127.0.0.1",
                              "root",
                              "root",
                              "cppbatis",
                              3306,
                              nullptr,
                              0);
  if (!res) {
    fprintf(stderr, "mysql_real_connect failed, error_no:%d, err_info:%s\n",
            mysql_errno(&mysql_), mysql_error(&mysql_));
    exit(0);  // 数据库异常中断
  }

  // 设置字符集
  if (mysql_set_character_set(&mysql_, "utf8")) {
    fprintf(stderr, "mysql_set_character_set failed, error:%s\n", mysql_error(&mysql_));
  }
}

Connection::~Connection() {

  // std::cout << ">>> construct connection" << std::endl;
    mysql_close(&mysql_);
}


MYSQL& Connection::GetMysqlInstance() {
  return mysql_;
}

