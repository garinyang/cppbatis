/*
 * desc: .
 * author: garin.yang
 * date: 2022/11/3
 */
#include "prepare_stmt.h"

PrepareStatement::PrepareStatement(const std::shared_ptr<Connection> conn_ptr):conn_(conn_ptr) {

  //std::cout << "[Constructor] PrepareStatement" << std::endl;

  mysql_stmt_ = mysql_stmt_init(&conn_->GetMysqlInstance());
  if (!mysql_stmt_)
  {
    fprintf(stderr, " mysql_stmt_init(), out of memory\n");
    exit(0);
  }

  // 清空 参数绑定buffer
  memset(bind_param_, 0x0, sizeof(MYSQL_BIND)*20);

  // 清空 结果绑定buffer
  memset(bind_result_, 0x0, sizeof(MYSQL_BIND)*50);
}

PrepareStatement::~PrepareStatement() {

  if (res_) {
    // 获取结果集中每一行包含的列数
    int field_nums = mysql_num_fields(res_);
    for (auto i = 0; i < field_nums; ++i) {
      if (bind_result_[i].buffer != nullptr) {
        free(bind_result_[i].buffer);
        bind_result_[i].buffer = nullptr;
      }
    }

    // 释放结果集对象
    mysql_free_result(res_);
  }
};

void PrepareStatement::Prepare(std::string sql) {

  if (mysql_stmt_prepare(mysql_stmt_, sql.c_str(), sql.length()))
  {
    fprintf(stderr, " mysql_stmt_prepare(), INSERT failed\n");
    fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
    return ;
  }
}

void PrepareStatement::SetInt(int index, long& value) {

  bind_param_[index].buffer_type = MYSQL_TYPE_LONG;
  //    int * int_value = (int*)malloc(sizeof(int));
  //    memset(int_value, 0x0, sizeof(int));
  //    memcpy(int_value, &value, sizeof(int));

  bind_param_[index].buffer = &value;
  bind_param_[index].is_null = 0;
  unsigned long len = sizeof(long);
  bind_param_[index].length = 0;
}

void PrepareStatement::SetString(int index, std::string& value) {

  bind_param_[index].buffer_type = MYSQL_TYPE_STRING;
  bind_param_[index].buffer= (char*)value.c_str();
  bind_param_[index].buffer_length= value.size();
}

std::map<std::string, std::pair<int, std::any>> PrepareStatement::ObtainMetaDataWithResBound() {

  std::map<std::string, std::pair<int, std::any>> mp_res;

  res_ = mysql_stmt_result_metadata(mysql_stmt_);
  if (!res_) {

  } else {
    unsigned int num_fields;
    unsigned int i;
    MYSQL_FIELD *fields;

    num_fields = mysql_num_fields(res_);
    fields = mysql_fetch_fields(res_);

    for (i = 0; i < num_fields; i++) {
      // fprintf(stdout, "Field %u is %s, %d\n", i, fields[i].name, fields[i].type);

      if (MYSQL_TYPE_LONG == fields[i].type) {
        auto int_value = (int64_t*)malloc(sizeof(int64_t));
        memset(int_value, 0x0, sizeof(int64_t));

        bind_result_[i].buffer_type   = MYSQL_TYPE_LONG;
        bind_result_[i].buffer        = int_value;
        bind_result_[i].buffer_length = 0;
        bind_result_[i].is_null       = nullptr;
        bind_result_[i].length        = 0;

        mp_res[fields[i].name] = std::make_pair(MYSQL_TYPE_LONG, int_value);
      } else if (MYSQL_TYPE_VAR_STRING == fields[i].type or MYSQL_TYPE_DATETIME == fields[i].type) {
        //          auto str_value = std::make_shared<char>(1024);
        // auto str_value = (char*)malloc(fields[i].max_length+1);
        auto str_value = (char*)malloc(fields[i].length + 1);
        //          std::cout << "malloc: " << fields[i].length << std::endl;
        memset(str_value, 0x0, fields[i].max_length + 1);

        bind_result_[i].buffer_type   = MYSQL_TYPE_STRING;
        bind_result_[i].buffer        = str_value;
        bind_result_[i].buffer_length = fields[i].length + 1;
        bind_result_[i].is_null       = nullptr;
        bind_result_[i].length        = 0;

        mp_res[fields[i].name] = std::make_pair(MYSQL_TYPE_VAR_STRING, str_value);
      } else if (MYSQL_TYPE_TIMESTAMP2 == fields[i].type or MYSQL_TYPE_LONGLONG == fields[i].type) {
        // 列为 时间戳（long long）
        auto longlong_value = (int64_t*)malloc(sizeof(int64_t));
        memset(longlong_value, 0x0, sizeof(int64_t));

        bind_result_[i].buffer_type   = MYSQL_TYPE_LONGLONG;
        bind_result_[i].buffer        = longlong_value;
        bind_result_[i].buffer_length = 0;
        bind_result_[i].is_null       = nullptr;
        bind_result_[i].length        = 0;

        mp_res[fields[i].name] = std::make_pair(MYSQL_TYPE_TIMESTAMP2, longlong_value);
      } else if (MYSQL_TYPE_DOUBLE == fields[i].type or MYSQL_TYPE_FLOAT == fields[i].type) {
        // fprintf(stderr, "[LOOK] UNHANDLED TYPE:%d\n", (int)fields[i].type);
        auto double_value = (double*)malloc(sizeof(double));
        memset(double_value, 0x0, sizeof(double));

        bind_result_[i].buffer_type   = MYSQL_TYPE_DOUBLE;
        bind_result_[i].buffer        = double_value;
        bind_result_[i].buffer_length = 0;
        bind_result_[i].is_null       = nullptr;
        bind_result_[i].length        = 0;

        mp_res[fields[i].name] = std::make_pair(MYSQL_TYPE_DOUBLE, double_value);
      }
    }
    /*
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
        //auto str_value = (char*)malloc(fields[i].max_length+1);
        auto str_value = (char*)malloc(fields[i].length+1);
        //          std::cout << "malloc: " << fields[i].length << std::endl;
        memset(str_value, 0x0, fields[i].max_length+1);

        bind_result_[i].buffer_type= MYSQL_TYPE_STRING;
        bind_result_[i].buffer= str_value;
        bind_result_[i].buffer_length= fields[i].length+1;
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
    }*/
  }

  // 绑定结果集
  if (mysql_stmt_bind_result(mysql_stmt_, bind_result_))
  {
    fprintf(stderr, " mysql_stmt_bind_result() failed\n");
    fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
  }

  return mp_res;
}

[[maybe_unused]] int PrepareStatement::GetAffactedRows() {
  int affected_rows = 0;
  affected_rows = mysql_stmt_affected_rows(mysql_stmt_);
  fprintf(stdout, " total affected rows(insert 1): %lu\n",
          (unsigned long) affected_rows);

  return affected_rows;
}

void PrepareStatement::BindParam() {
  if (mysql_stmt_bind_param(mysql_stmt_, bind_param_))
  {
    fprintf(stderr, " mysql_stmt_bind_param() failed\n");
    fprintf(stderr, " %s\n", mysql_stmt_error(mysql_stmt_));
    return;
  }

  auto param_count= mysql_stmt_param_count(mysql_stmt_);
  std::cout << "param_count: " << param_count << std::endl;
}

void PrepareStatement::Close() {

  if (nullptr == mysql_stmt_) {
    return ;
  }

  if (mysql_stmt_close(mysql_stmt_))
  {
    /* mysql_stmt_close() invalidates stmt, so call          */
    /* mysql_error(mysql) rather than mysql_stmt_error(stmt) */
    fprintf(stderr, " failed while closing the statement\n");
    fprintf(stderr, " %s\n", mysql_error(&conn_->GetMysqlInstance()));
    return ;
  }
}