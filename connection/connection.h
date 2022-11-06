/*
* desc: encapsulation for connection to mysql.
* author: garin.yang
* date: 2022-09-21.
*/

#ifndef CPPBATIS_CONNECTION_H
#define CPPBATIS_CONNECTION_H

#include <iostream>
#include <mysql/mysql.h>

class Connection {
public:

  Connection();

  ~Connection();

  MYSQL& GetMysqlInstance();

private:

  std::string host_;
  unsigned port_;
  std::string user;
  std::string passwd;

  MYSQL mysql_; // mysql 连接对象

};


#endif // CPPBATIS_CONNECTION_H
