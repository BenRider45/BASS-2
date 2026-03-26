#pragma once

#include <QString>

template <typename T> class Result {
public:
  explicit Result(T value) : _success(true), _value(value), _error("") {};

  explicit Result(QString error, int errorCode)
      : _success(false), _value(T()), _error(error), _errorCode(errorCode) {};

  [[nodiscard]] bool isSuccess() const { return _success; }
  [[nodiscard]] bool isError() const { return !_success; }

  T &value() {
    Q_ASSERT(_success);
    return _value;
  }

  [[nodiscard]] QString error() const { return _error; }

  static Result<T> ok(T value) { return Result<T>(value); }

  static Result<T> err(QString message, int errorCode) {
    return Result<T>(message, errorCode);
  }

  template <typename U, typename Fn> Result<U> map(Fn f) const {
    if (isError()) {
      return Result<U>::err(_error);
    }
    try {
      return Result<U>::ok(f(_value));
    } catch (const std::exception &e) {
      return Result<U>::err(QString::fromStdString(e.what()));
    }
  }

private:
  bool _success;
  T _value;
  QString _error;
  int _errorCode;
};
