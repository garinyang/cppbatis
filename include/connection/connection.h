/*
 * @Author: garin.yang garin.yang@outlook.com
 * @Date: 2023-04-15 21:27:28
 * @LastEditors: garin.yang garin.yang@outlook.com
 * @LastEditTime: 2023-04-15 22:12:12
 * @FilePath: /cppbatis-1/connection/connection.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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

  MYSQL* GetMysqlInstance();

private:

  std::string host_;
  unsigned port_;
  std::string user;
  std::string passwd;

  MYSQL* mysql_ = nullptr; // mysql 连接对象

};


#endif // CPPBATIS_CONNECTION_H
