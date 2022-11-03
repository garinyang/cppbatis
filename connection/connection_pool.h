/*
 * desc: 数据库连接池封装.
 * author: garin.yang
 * date: 2022/10/29
 */

#ifndef CPPBATIS_CONNECTION_POOL_H
#define CPPBATIS_CONNECTION_POOL_H

#include <iostream>
#include <list>
#include <memory>

#include "singleton_base.h"
#include "connection.h"

#define MAX_CONN_NUM 16
#define DEFAULT_CONN_NUM 8

class ConnectPool :public SingletonBase<ConnectPool> {

public:

  /*
     * 获取数据库链接
     * */
  const std::shared_ptr<Connection> GetConnection() {

    if (0 == conn_list_.size()) {
      fprintf(stderr, "no free connection left!!!");
      return nullptr;
    }

    auto& conn = conn_list_.front();
    conn_list_.pop_front();

    return conn;
  }

  /*
     * 释放数据库链接
     * */
  void ReleaseConnection(const std::shared_ptr<Connection>& conn) {

    if (conn != nullptr) {
      conn_list_.emplace_back(std::move(conn));
    } else {
      std::cout << "invalid released connection handle!!!" << std::endl;
    }
  }

  /*
     * 获取 空闲链接
     * */

  unsigned int GetFreeConnNum() const {
    return conn_list_.size();
  }

  /*
     * 获取最大链接
     *
     * */

  unsigned int GetMaxConnNum() const {
    return max_conn_;
  }

  ConnectPool(int pool_size = DEFAULT_CONN_NUM) {
    //fprintf(stdout, "Construct the connection pool, default pool size=8\n");
    InitPool(pool_size);
  }

protected:

  bool InitPool(int pool_size = DEFAULT_CONN_NUM) {

    //std::cout << "InitPool" << std::endl;

    for (int i = 0; i < pool_size; ++i) {

      auto conn = std::make_shared<Connection>();
      conn_list_.emplace_back(conn);
    }

    return true;
  }


private:

  unsigned int max_conn_ = MAX_CONN_NUM;

  std::list<std::shared_ptr<Connection>> conn_list_;

};


#endif // CPPBATIS_CONNECTION_POOL_H
