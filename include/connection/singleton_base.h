/*
 * @Author: garin.yang garin.yang@outlook.com
 * @Date: 2023-04-15 21:27:28
 * @LastEditors: garin.yang garin.yang@outlook.com
 * @LastEditTime: 2023-04-15 21:56:08
 * @FilePath: /cppbatis-1/connection/singleton_base.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * desc: 单例模板
 * author: garin.yang
 * date: 2022/10/29
 */

#ifndef CPPBATIS_SINGLETON_BASE_H
#define CPPBATIS_SINGLETON_BASE_H

template <typename T>
class SingletonBase {

public:

  static T& ObtainInstance() {
    static T inst;
    return inst;
  }

protected:

  SingletonBase() = default;

  ~SingletonBase() = default;
};



#endif // CPPBATIS_SINGLETON_BASE_H
