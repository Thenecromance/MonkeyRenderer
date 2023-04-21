#pragma once

#include <memory>

template <typename Type>
class LazySingleTon {
 public:
  static std::shared_ptr<Type> &GetInstance() {
    if (pInstance == nullptr) {
      pInstance = std::make_shared<Type>();
    }
    return pInstance;
  }

  LazySingleTon(const Type &) = delete;

  LazySingleTon(Type &&) = delete;

  void operator=(const Type &) = delete;

 protected:
  LazySingleTon() = default;

  virtual ~LazySingleTon() = default;

 private:
  static std::shared_ptr<Type> pInstance;
};

template <typename Type>
std::shared_ptr<Type> LazySingleTon<Type>::pInstance = nullptr;

template <typename Type>
class EagerSingleTon {
 public:
  static Type &GetInstance() {
    static Type instance;
    return instance;
  }

  EagerSingleTon(Type &&) = delete;

  EagerSingleTon(const Type &) = delete;

  void operator=(const Type &) = delete;

 protected:
  EagerSingleTon() = default;

  virtual ~EagerSingleTon() = default;
};

#define LAZYSingleTon(obj) class obj : public LazySingleTon<obj>
#define EAGERSingleTon(obj) class obj : public EagerSingleTon<obj>