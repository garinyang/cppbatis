/*
 * desc: 数据库连接池封装.
 * author: garin.yang
 * date: 2022/10/29
 */

#ifndef CPPBATIS_CONNECTION_POOL_H
#define CPPBATIS_CONNECTION_POOL_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <list>
#include <fstream>

#include "reflection/easy_json.h"
#include "singleton_base.h"
#include "connection/connection.h"

#define MAX_CONN_NUM 16
#define DEFAULT_CONN_NUM 8

struct MysqlOption {
  MysqlOption() = default;
  std::string host {"127.0.0.1"};
  std::uint16_t port {3306};
  std::string user {"root"};
  std::string passwd {"root"};
  std::string db {"test"};
  JSON_HELP(MysqlOption, host, port, user, passwd, db);
};

class ConnectPool : public SingletonBase<ConnectPool> {
public:
  ConnectPool() {
    //fprintf(stdout, "Construct the connection pool, default pool size=8\n");
    InitPool();
  }

  ~ConnectPool() {
    //fprintf(stdout, "Destruct the connection pool\n");
  }

  /*
     * 获取数据库链接
     * */
  const std::shared_ptr<Connection> GetConnection() {
    std::lock_guard<std::mutex> lock(mutex_);

    // CASE 1: 若连接池中有空闲连接, 则直接返回
    while (!conn_list_.empty()) {
      auto conn = conn_list_.front();
      conn_list_.pop_front();

      // 检查连接是否有效, 若有效则返回, 否则释放连接并重新创建
      if (conn->IsAlive()) {
        return conn;
      }
      else {
        conn.reset();
        conn_list_.emplace_back(std::make_shared<Connection>(option_.host, option_.port, option_.user, option_.passwd, option_.db));
        fprintf(stderr, "connection is not alive, release it!!!");
      }
    }

    // CASE 2: 若连接池中无空闲连接, 则返回 nullptr
    fprintf(stderr, "no free connection left!!!");
    return nullptr;
  }

  /*
     * 释放数据库链接
     * */
  void ReleaseConnection(const std::shared_ptr<Connection>& conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (conn != nullptr) {
      conn_list_.emplace_back(std::move(conn));
    } else {
      std::cout << "invalid released connection handle!!!" << std::endl;
    }
  }

  /*
     * 获取 空闲链接
     * */

  unsigned int GetFreeConnNum() {
    std::lock_guard<std::mutex> lock(mutex_);
    return conn_list_.size();
  }

protected:

  bool InitPool() {
    InitConfig();

    for (int i = 0; i < DEFAULT_CONN_NUM; ++i) {
      auto conn = std::make_shared<Connection>(option_.host, option_.port, option_.user, option_.passwd, option_.db);
      conn_list_.emplace_back(conn);
    }

    return true;
  }

  bool InitConfig() {
    nlohmann::json data;
    try {
        std::ifstream ifs("../conf/cppbatis_config.json", std::ios::in);
        ifs >> data;
    } catch (const std::exception& e) {
        std::cerr << "[CPPBATIS]Failed to load config file in ../conf/cppbatis_config.json" << ": " << e.what() << std::endl;
        return false;
    }

    // 从配置文件中读取数据库配置
    if (data.is_object() && data.contains("mysql") && data["mysql"].is_object()) {
      nlohmann::json mysql_config = data["mysql"];
      option_.unmarshal(mysql_config);
    }
    std::cout << "InitConfig: host=" << option_.host << ", port=" << option_.port << ", user=" << option_.user << ", passwd=" << option_.passwd << ", db=" << option_.db << std::endl;

    return true;
  }

private:
  std::list<std::shared_ptr<Connection>> conn_list_;
  MysqlOption option_;
  std::mutex mutex_;
};

#endif // CPPBATIS_CONNECTION_POOL_H
