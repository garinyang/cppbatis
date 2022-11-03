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
#include "reflection/easy_json.h"

class PrepareStatement {

public:

  PrepareStatement() = default;

  PrepareStatement(const std::shared_ptr<Connection> conn_ptr):conn_(conn_ptr) {
    std::cout << "[Constructor] PrepareStatement" << std::endl;

    mysql_stmt_ = mysql_stmt_init(conn_->GetMysqlInstance());
    if (!mysql_stmt_)
    {
      fprintf(stderr, " mysql_stmt_init(), out of memory\n");
      exit(0);
    }

    // 清空 参数绑定buffer
    memset(bind_param_, 0, sizeof(bind_param_));

    // 清空结果绑定buffer
    memset(bind_result_, 0, sizeof(bind_result_));
  }

  ~PrepareStatement() {

    int field_nums = mysql_num_fields(res_);

    mysql_free_result(res_);

    for (auto i = 0; i <field_nums; ++i) {
      if (bind_result_[i].buffer != nullptr) {
        free(bind_result_[i].buffer);
        bind_result_[i].buffer = nullptr;
      }
    }
  };

  void Prepare(std::string sql) {

    if (mysql_stmt_prepare(mysql_stmt_, sql.c_str(), sql.length()))
    {
      fprintf(stderr, " mysql_stmt_prepare(), INSERT failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
      exit(0);
    }
    // fprintf(stdout, " prepare, INSERT successful\n");
  }

  void SetInt(int index, long& value) {

    bind_param_[index].buffer_type = MYSQL_TYPE_LONG;
    bind_param_[index].buffer = (char *)&value;
    bind_param_[index].is_null = 0;
    bind_param_[index].length = 0;

    BindParam();
  }

  void SetShort(int index, short value) {

    bind_param_[index].buffer_type= MYSQL_TYPE_SHORT;
    bind_param_[index].buffer= (char *)&value;
    //bind[2].is_null= &is_null;
    bind_param_[index].is_null= 0;
    bind_param_[index].length= 0;

    BindParam();
  }

  void SetString(int index, std::string value) {

    bind_param_[index].buffer_type= MYSQL_TYPE_STRING;
    bind_param_[index].buffer= (char *)value.data();
    bind_param_[index].buffer_length= value.length();
    bind_param_[index].is_null= nullptr;
    unsigned long len = value.length();
    bind_param_[index].length= &len;

    BindParam();
  }

  //    void SetTime(int index, int value) {
  //
  //        bind_param_[index].buffer_type= MYSQL_TYPE_STRING;
  //        bind_param_[index].buffer= (char *)str_data;
  //        bind_param_[index].buffer_length= STRING_SIZE;
  //        bind_param_[index].is_null= 0;
  //        bind_param_[index].length= &str_length;
  //
  //        BindParam();
  //    }

  /*
     * 查询
     * */
  template <class T>
  void Query(std::vector<T>& obj, std::map<std::string, std::pair<int, std::any>>& res) {

    // 1. 执行
    if (mysql_stmt_execute(mysql_stmt_))
    {
      fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
      exit(0);
    }

    // 获取结果
    int status;
    while (true)
    {
      // store res
      // if ( 0 != mysql_stmt_store_result(mysql_stmt_) ) {
      // fprintf(stderr, "mysql_stmt_store_result failed, info:%s\n", mysql_stmt_error(mysql_stmt_));
      // }

      // fetch each col
      status = mysql_stmt_fetch(mysql_stmt_);
      //fprintf(stdout, "mysql_stmt_fetch, status:%d\n", status);
      if (status == 1 || status == MYSQL_NO_DATA) {
        //fprintf(stdout, "### MYSQL_NO_DATA ###\n");
        break;
      }
      /* handle current row here */
      //           printf("handle current row here, %d,%s,%s,%d \n", *(int*)bind_result_[0].buffer, (char*)bind_result_[1].buffer,
      //                   (char*)bind_result_[2].buffer, *(int*)bind_result_[3].buffer);

      Json::Value jres;
      for (auto it = res.begin(); it != res.end(); ++it) {

          // 分拣处理不同类型
          if (MYSQL_TYPE_LONG == it->second.first) {
            //std::cout << it->first << ":" << *std::any_cast<int64_t *>(it->second.second)<< " ";
            jres[it->first] = int(*std::any_cast<int64_t *>(it->second.second)); // 精度丢失


          } else if (MYSQL_TYPE_VAR_STRING == it->second.first) {
            //std::cout << it->first << ":" <<std::any_cast<std::shared_ptr<char>>(it->second.second) << " ";
            //std::cout << it->first << ":" <<std::any_cast<char*>(it->second.second) << " ";
            jres[it->first] = (std::string)std::any_cast<char*>(it->second.second);

          }  else if (MYSQL_TYPE_TIMESTAMP2 == it->second.first){
            //std::cout << it->first << ":" << *std::any_cast<int64_t *>(it->second.second) <<std::endl;
            jres[it->first] = (int)*std::any_cast<int64_t *>(it->second.second);
          }
      }
      //std::cout << std::endl;

      /************** 通用类型 ***************/
      // 返回值类型：泛型T
      // 返回对象不确定： 反射

      T t;
      t.unmarshal(jres);

      obj.emplace_back(std::move(t));

    }
  }

  /*
     * 更新
     * */
  bool Update() {

    if (mysql_stmt_execute(mysql_stmt_))
    {
      fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
      exit(0);
    }

    return true;
  }


  std::map<std::string, std::pair<int, std::any>> GetMetaData() {

    std::map<std::string, std::pair<int, std::any>> mp_res;

    res_ = mysql_stmt_result_metadata(mysql_stmt_);
    if (!res_) {

    } else {
      unsigned int num_fields;
      unsigned int i;
      MYSQL_FIELD *fields;

      num_fields = mysql_num_fields(res_);
      fields = mysql_fetch_fields(res_);

      for(i = 0; i < num_fields; i++)
      {
        // fprintf(stdout, "Field %u is %s, %d\n", i, fields[i].name, fields[i].type);

        if ( MYSQL_TYPE_LONG == fields[i].type) {
          auto int_value = (int64_t*)malloc(sizeof(int64_t));
          memset(int_value, 0x0, sizeof(int64_t));

          bind_result_[i].buffer_type= MYSQL_TYPE_LONG;
          bind_result_[i].buffer= int_value;
          bind_result_[i].buffer_length= 0;
          bind_result_[i].is_null= nullptr;
          bind_result_[i].length= 0;

          mp_res[fields[i].name] = std::make_pair(MYSQL_TYPE_LONG,int_value);
        } else if ( MYSQL_TYPE_VAR_STRING == fields[i].type) {
//          auto str_value = std::make_shared<char>(1024);
          auto str_value = (char*)malloc(fields[i].max_length+1);
          memset(str_value, 0x0, fields[i].max_length+1);

          bind_result_[i].buffer_type= MYSQL_TYPE_STRING;
          bind_result_[i].buffer= str_value;
          bind_result_[i].buffer_length= 64;
          bind_result_[i].is_null= nullptr;
          bind_result_[i].length= 0;

          mp_res[fields[i].name] = std::make_pair(MYSQL_TYPE_VAR_STRING, str_value);
        } else if ( MYSQL_TYPE_TIMESTAMP2 == fields[i].type){
          // 列为 时间戳（long long）

          auto ts = (int64_t*)malloc(sizeof(int64_t));
          memset(ts, 0x0, sizeof(int64_t));

          bind_result_[i].buffer_type= MYSQL_TYPE_TIMESTAMP2;
          bind_result_[i].buffer= ts;
          bind_result_[i].buffer_length= 0;
          bind_result_[i].is_null= nullptr;
          bind_result_[i].length= 0;

          mp_res[fields[i].name] = std::make_pair(MYSQL_TYPE_TIMESTAMP2, ts);
        }
      }
    }

    if (mysql_stmt_bind_result(mysql_stmt_, bind_result_))
    {
      fprintf(stderr, " mysql_stmt_bind_result() failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
      exit(0);
    }

    return mp_res;
  }

  /*
     * 获取影响行数
     * */
  int GetAffactedRows() {

    int affected_rows = mysql_stmt_affected_rows(mysql_stmt_);
    fprintf(stdout, " total affected rows(insert 1): %lu\n",
            (unsigned long) affected_rows);

    return affected_rows;
  }

  void Close() {

    if (mysql_stmt_close(mysql_stmt_))
    {
      /* mysql_stmt_close() invalidates stmt, so call          */
      /* mysql_error(mysql) rather than mysql_stmt_error(stmt) */
      fprintf(stderr, " failed while closing the statement\n");
      fprintf(stderr, " %s\n", mysql_error(conn_->GetMysqlInstance()));
      exit(0);
    }
  }

//  void BindResult(User& user) {
//
//    // id
//    bind_result_[0].buffer_type= MYSQL_TYPE_LONG;
//    bind_result_[0].buffer= (char *)&user.id;
//    bind_result_[0].buffer_length= 0;
//    bind_result_[0].is_null= nullptr;
//    bind_result_[0].length= 0;
//
//    // name
//    bind_result_[1].buffer_type= MYSQL_TYPE_STRING;
//    bind_result_[1].buffer= (char *)user.name;
//    bind_result_[1].buffer_length= 64;
//    bind_result_[1].is_null= nullptr;
//    bind_result_[1].length= 0;
//
//    // email
//    bind_result_[2].buffer_type= MYSQL_TYPE_STRING;
//    bind_result_[2].buffer= (char *)user.email;
//    bind_result_[2].buffer_length= 128;
//    bind_result_[2].is_null= nullptr;
//    bind_result_[2].length= 0;
//
//    // age
//    bind_result_[3].buffer_type= MYSQL_TYPE_LONG;
//    bind_result_[3].buffer= (char *)&user.age;
//    bind_result_[3].buffer_length= 0;
//    bind_result_[3].is_null= nullptr;
//    bind_result_[3].length= 0;
//
//    if (mysql_stmt_bind_result(mysql_stmt_, bind_result_))
//    {
//      fprintf(stderr, " mysql_stmt_bind_result() failed\n");
//      fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
//      exit(0);
//    }
//
//    printf("bind result success.\n");
//  }
protected:

  void BindParam() {

    if (mysql_stmt_bind_param(mysql_stmt_, bind_param_))
    {
      fprintf(stderr, " mysql_stmt_bind_param() failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
      exit(0);
    }

  }

private:

  std::shared_ptr<Connection> conn_;

  MYSQL_STMT* mysql_stmt_;

  MYSQL_RES * res_;

  MYSQL_BIND bind_param_[20];
  MYSQL_BIND bind_result_[50];
};



#endif // CPPBATIS_PREPARE_STMT_H
