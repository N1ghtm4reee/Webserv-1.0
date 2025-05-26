#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

class Request {
public:
    Request();
    ~Request();

    int parse(const std::string &raw_request);
    int isKeys(const std::string &key);
    bool isComplete(const std::string &buffer);

    void setPath(const std::string &path);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getQuery() const;
    std::string getHttpVersion() const;
    std::string getHeader(const std::string &key);
    std::string getBody();

private:
    std::map<std::string, std::string> headers;
    std::string method;
    std::string path;
    std::string query;
    std::string httpVersion;
    std::string body;
    int statusCode;

    void parseRequestLine(const std::string &line);
    void parseHeaders(const std::string &line);
    void parseBody(const std::string &line);
};

#endif