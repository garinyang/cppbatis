# MYSQL数据库客户端框架 CppBatis 
- A orm database structure for mysql in c++, just like mybatis for java.

MYSQL数据库客户端orm框架，思路参考的是高级语言中的orm思想，比如：python中的archery，java中的mybatis/plus.
应该算是所有通用实现里，代码最为简洁的一版；（该项目构建与MacOS环境，在linux环境下，cmake可能是否做些许调整；）

[使用说明]

- 目前支持 单表查询，每个 struct 映射 一张表；（`注意：struct字段名 必须与 表中 field名称一样！！！！`）

[实现说明]
![img.png](img.png)
1. 基于 mysql prepare-statement statement C接口封装，支持简单的连接管理； （MySQL Connector/C 8.0）
2. 开源依赖项：
- 通用性考虑，MySQL官方C-API: [https://dev.mysql.com/doc/c-api/8.0/en/c-api-prepared-statement-interface.html](https://dev.mysql.com/doc/c-api/8.0/en/c-api-prepared-statement-interface.html)
- 静态反射：[https://github.com/yxSunRise/EasyJson.git](https://github.com/yxSunRise/EasyJson.git)
- jsoncpp-1.9.5：[https://github.com/open-source-parsers/jsoncpp.git](https://github.com/yxSunRise/EasyJson.git)

[计划]
* 第一步：[已完成] 基础preparestatement C接口封装一个简单的msyql连接客户端，支持简单的 "更新"，"查询"；
* 第二步：[待完成] 结合模板，改造"更新"&"查询"功能，实现"通用"的数据结构查询&更新功能；[已支持 "查询]
* 第三步：[待完成] 支持事务，联合查询等；


---
作者: garin.yang
联系方式: garin.yang@outlook.com


