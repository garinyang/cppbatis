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
