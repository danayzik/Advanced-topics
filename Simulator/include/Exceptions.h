#pragma once
#include <exception>
#include <string>

class BadRegistrationException : public std::exception{
private:
    std::string message;

public:
    explicit BadRegistrationException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};


class SharedObjectLoadingException : public std::exception{
private:
    std::string message;

public:
    explicit SharedObjectLoadingException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};


class MapLoadingException : public std::exception{
private:
    std::string message;

public:
    explicit MapLoadingException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class NotEnoughValidMaps : public std::exception{
private:
    std::string message;

public:
    explicit NotEnoughValidMaps(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class NotEnoughValidAlgorithms : public std::exception{
private:
    std::string message;

public:
    explicit NotEnoughValidAlgorithms(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};


