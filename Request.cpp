#include "Request.hpp"
#include <sstream>
#include <iostream>
#include <cctype>

Request::Request() {}

Request::~Request() {}

void Request::parseRequestLine(const std::string &line)
{
    std::cout << "request line: " << line << std::endl;
    std::istringstream stream(line);
    std::string method, path, httpVersion;
    stream >> method >> path >> httpVersion;

    this->method = method;
    this->path = path;
    this->httpVersion = httpVersion;

    if (method.empty() || path.empty() || httpVersion.empty())
        throw 400;
    if (method != "GET" && method != "POST" && method != "DELETE")
        throw 501;
    if (httpVersion != "HTTP/1.1")
        throw 400;
}



int Request::parse(const std::string &raw_request)
{
    std::istringstream stream(raw_request);
    std::string line;
    std::getline(stream, line);

    try{
            // Parse request line
            parseRequestLine(line);
            // Parse headers
            while (std::getline(stream, line))
            {
                std::cout << "line : " << line << std::endl;
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
        
                if (line.empty())
                    break;
        
                std::size_t pos = line.find(':');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
        
                    value.erase(0, value.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    headers[key] = value;
                }
            }
            // parse body
            if (method == "POST")
            {
                if (headers.find("Content-Type") != headers.end() && headers["Content-Type"].find("multipart") != std::string::npos)
                    throw 501;
                else if (headers.find("Content-Length") != headers.end() && !headers["Content-Length"].empty())
                {
                    int content_length = std::stoi(headers["Content-Length"]);
                    char *buffer = new char[content_length];
                    stream.read(buffer, content_length);
                    body.assign(buffer, content_length);
                    delete[] buffer;
                }
                if (headers["Transfer-Encoding"] == "chunked")
                {
                    std::string line;
                    while (std::getline(stream, line))
                    {
                        if (!line.empty() && line.back() == '\r')
                            line.pop_back();

                        int chunk_size = std::stoi(line, nullptr, 16);
                        if (chunk_size == 0)
                            break;

                        char *chunk_data = new char[chunk_size];
                        stream.read(chunk_data, chunk_size);
                        body.append(chunk_data, chunk_size);
                        delete[] chunk_data;

                        std::getline(stream, line);
                    }
                }
                else if (!headers["Content-Length"].empty())
                {
                    int content_length = std::stoi(headers["Content-Length"]);
                    char *buffer = new char[content_length];
                    stream.read(buffer, content_length);
                    body.assign(buffer, content_length);
                    delete[] buffer;
                }
                else
                    throw 400;
            }
    }
    catch (int code)
    {
        return code;
    }
    return 200;
}

std::string Request::getMethod() const
{
    return method;
}
std::string Request::getPath() const
{
    return path;
}
std::string Request::getQuery() const
{
    std::size_t pos = path.find('?');
    if (pos != std::string::npos)
        return path.substr(pos + 1);
    return "";
}
std::string Request::getHttpVersion() const
{
    return httpVersion;
}
std::string Request::getHeader(const std::string &key)
{
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end())
        return it->second;
    return "";
}

std::string Request::getBody()
{
    return body;
}

bool Request::isComplete(const std::string &buffer)
{
    size_t headers_end = buffer.find("\r\n\r\n");
    if (headers_end == std::string::npos)
        return false;

    size_t body_start = headers_end + 4;
    size_t content_length = 0;

    size_t cl_pos = buffer.find("Content-Length:");
    if (cl_pos != std::string::npos)
    {
        size_t cl_end = buffer.find("\r\n", cl_pos);
        std::string cl_value = buffer.substr(cl_pos + 15, cl_end - (cl_pos + 15));
        content_length = std::atoi(cl_value.c_str());
    }

    if (content_length > 0)
        return buffer.size() >= body_start + content_length;

    return true;
}

void Request::setPath(const std::string &path)
{
    this->path = path;
}
