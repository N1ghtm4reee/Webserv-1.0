#include "Response.hpp"

std::string getResponseLine(int statusCode)
{
    switch (statusCode)
    {
    case 200:
        return "HTTP/1.1 200 OK\r\n";
    case 400:
        return "HTTP/1.1 400 Bad Request\r\n";
    case 401:
        return "HTTP/1.1 401 Unauthorized\r\n";
    case 403:
        return "HTTP/1.1 403 Forbidden\r\n";
    case 404:
        return "HTTP/1.1 404 Not Found\r\n";
    case 405:
        return "HTTP/1.1 405 Method Not Allowed\r\n";
    case 500:
        return "HTTP/1.1 500 Internal Server Error\r\n";
    default:
        return "HTTP/1.1 500 Internal Server Error\r\n";
    }
}

std::string readFile(const std::string &path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Unable to open file: " + path);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Response::sendErrorPage(int fd, Request &Req, int statusCode, Server &server)
{
    int foundPage = 0;

    std::cout << "status code throwd : " << statusCode << std::endl;
    std::map<int, std::string> page = server.getErrorPages();
    if (page.find(statusCode) != page.end())
    {
        std::string errorPage = page[statusCode];
        if (access(errorPage.c_str(), F_OK) != -1 && access(errorPage.c_str(), R_OK) != -1)
        {
            int file_fd = open(errorPage.c_str(), O_RDONLY);
            if (file_fd < 0)
            {
                perror("open");
                return;
            }
            std::string body = readFile(errorPage);
            std::string ResponseLine = getResponseLine(statusCode);
            std::string response =
                ResponseLine +
                "Content-Length: " + std::to_string(body.size()) + "\r\n"
                                                                   "Content-Type: text/html; charset=UTF-8\r\n"
                                                                   "Connection: close\r\n"
                                                                   "\r\n" +
                body;
            foundPage = 1;
            send(fd, response.c_str(), response.size(), 0);
        }
    }
    if (foundPage == 0)
    {
        std::string ResponseLine = getResponseLine(statusCode);

        std::string body = "<!DOCTYPE html>\n"
                           "<html>\n"
                           "<head><title>My Server</title></head>\n"
                           "<body style=\"background-color: black; color: white;\">\n"
                           "  <h1>" +
                           std::to_string(statusCode) + " Error</h1>\n"
                                                        "</body>\n"
                                                        "</html>\n";
        std::string response =
            ResponseLine +
            "Content-Length: " +
            std::to_string(body.size()) + "\r\n"
                                          "Content-Type: text/html; charset=UTF-8\r\n"
                                          "Connection: keep-alive\r\n"
                                          "\r\n" +
            body;
        ssize_t sent = send(fd, response.c_str(), response.size(), 0);
    }
}

bool endsWith(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string getMimeType(const std::string &path)
{
    if (endsWith(path, ".html") || endsWith(path, ".htm"))
        return "text/html";
    if (endsWith(path, ".css"))
        return "text/css";
    if (endsWith(path, ".js"))
        return "application/javascript";
    if (endsWith(path, ".png"))
        return "image/png";
    if (endsWith(path, ".jpg") || endsWith(path, ".jpeg"))
        return "image/jpeg";
    if (endsWith(path, ".gif"))
        return "image/gif";
    if (endsWith(path, ".ico"))
        return "image/x-icon";
    if (endsWith(path, ".svg"))
        return "image/svg+xml";
    if (endsWith(path, ".mp3"))
        return "audio/mpeg";
    if (endsWith(path, ".mp4"))
        return "video/mp4";
    return "application/octet-stream";
}

void sendPage(int fd, int statusCode, std::string path, std::string Connection)
{
    int file_fd = open(path.c_str(), O_RDONLY);
    if (file_fd < 0)
    {
        perror("open");
        return;
    }
    std::string body = readFile(path);
    std::string ResponseLine = getResponseLine(statusCode);
    std::string mimeType = getMimeType(path);
    std::string response =
        ResponseLine +
        "Content-Length: " + std::to_string(body.size()) + "\r\n" +
        "Content-Type: " + mimeType + "\r\n" +
        "Connection: " + Connection + "\r\n"
                                      "\r\n" +
        body;

    send(fd, response.c_str(), response.size(), 0);
}

Location Response::getLocation(std::string path, std::vector<Location> Locations)
{
    Location location;
    int found = 0;
    size_t maxLength = 0;

    for (size_t i = 0; i < Locations.size(); i++)
    {
        const std::string &locationPath = Locations[i].getPath();
        if (path.find(locationPath) == 0)
        {
            if (path.length() == locationPath.length() ||
                locationPath.back() == '/' ||
                (path.length() > locationPath.length() && path[locationPath.length()] == '/'))
            {
                if (locationPath.length() > maxLength)
                {
                    found = 1;
                    location = Locations[i];
                    maxLength = locationPath.length();
                }
            }
        }
    }
    if (!found)
        throw 404;
    return location;
}

bool isAllowedMethod(std::string method, std::vector<std::string> allowedMethods)
{
    std::cout << "Method: " << method << std::endl;
    std::cout << "Allowed Methods: ";
    for (size_t i = 0; i < allowedMethods.size(); i++)
        std::cout << allowedMethods[i] << " ";
    for (size_t i = 0; i < allowedMethods.size(); i++)
    {
        if (allowedMethods[i] == method)
        {
            std::cout << "Method allowed: " << method << std::endl;
            return true;
        }
    }
    return false;
}

bool hasRedirection(std::map<int, std::string> redirections)
{
    if (redirections.empty())
        return false;
    return true;
}

void sendRedirectionResponse(int fd, const std::map<int, std::string> &redirections,
                             const std::string &oldPath, int statusCode)
{
    std::cout << "sending redirection response" << std::endl;

    // Find the appropriate redirection based on status code
    auto it = redirections.find(statusCode);
    if (it == redirections.end())
    {
        // If specific status code not found, use the first available redirection
        it = redirections.begin();
    }

    std::string newPath;

    // Check if the redirection is a full URL or a relative path
    if (it->second.find("http://") == 0)
    {
        // Full URL redirection
        newPath = it->second;
    }
    else
    {
        // Relative path redirection
        std::string basePath = oldPath;

        // Remove trailing slash if present
        if (!basePath.empty() && basePath.back() == '/')
        {
            basePath.erase(basePath.size() - 1);
        }

        // Handle different redirection path formats
        if (it->second.front() == '/')
        {
            // Absolute path from root
            newPath = it->second;
        }
        else
        {
            // Relative path from current location
            newPath = basePath + "/" + it->second;
        }
    }

    std::cout << "old path: " << oldPath << std::endl;
    std::cout << "new path: " << newPath << std::endl;

    // Determine the appropriate status code text
    std::string statusText;
    switch (statusCode)
    {
    case 301:
        statusText = "Moved Permanently";
        break;
    case 302:
        statusText = "Found";
        break;
    case 303:
        statusText = "See Other";
        break;
    case 307:
        statusText = "Temporary Redirect";
        break;
    case 308:
        statusText = "Permanent Redirect";
        break;
    default:
        statusText = "Moved Permanently";
        break;
    }

    std::string response = "HTTP/1.1 " + std::to_string(statusCode) + " " + statusText + "\r\n" +
                           "Location: " + newPath + "\r\n" +
                           "Content-Length: 0\r\n" +
                           "Connection: close\r\n" +
                           "\r\n";

    ssize_t bytesSent = send(fd, response.c_str(), response.size(), 0);
    if (bytesSent == -1)
    {
        std::cerr << "Error sending redirection response" << std::endl;
    }
}

void setProperRequestPath(Request &Req, std::string &rootPath, std::string &locationPath)
{
    std::string uri = Req.getPath();
    if (uri.length() > locationPath.length())
    {
        std::string remainingPath = uri.substr(locationPath.length());
        if (!rootPath.empty() && rootPath.back() == '/' &&
            !remainingPath.empty() && remainingPath[0] == '/')
            remainingPath = remainingPath.substr(1);
        else if (!rootPath.empty() && rootPath.back() != '/' &&
                 !remainingPath.empty() && remainingPath[0] != '/')
            rootPath += "/";
        Req.setPath(rootPath + remainingPath);
    }
    else
        Req.setPath(rootPath);
}

void PostResponse(Request Req, Location &location, int fd, Server &server)
{
    std::string body = Req.getBody();
    std::string contentType = Req.getHeader("Content-Type");

    // Parse multipart form data if needed
    std::string fileData;
    std::string filename = "uploaded_file";
    std::string actualContentType = "";

    if (contentType.find("multipart/form-data") != std::string::npos)
    {
        // Extract boundary
        size_t boundaryPos = contentType.find("boundary=");
        if (boundaryPos == std::string::npos)
        {
            std::cerr << "No boundary found in multipart data" << std::endl;
            throw 400;
        }

        std::string boundary = "--" + contentType.substr(boundaryPos + 9);

        // Find file part (simplified parsing)
        size_t fileStart = body.find("Content-Disposition: form-data");
        if (fileStart == std::string::npos)
        {
            std::cerr << "No form-data found" << std::endl;
            throw 400;
        }

        // Extract filename
        size_t filenamePos = body.find("filename=", fileStart);
        if (filenamePos != std::string::npos)
        {
            size_t start = body.find('"', filenamePos) + 1;
            size_t end = body.find('"', start);
            if (start != std::string::npos && end != std::string::npos)
            {
                filename = body.substr(start, end - start);
            }
        }

        // Extract actual content type
        size_t ctPos = body.find("Content-Type:", fileStart);
        if (ctPos != std::string::npos)
        {
            size_t start = ctPos + 14;
            size_t end = body.find("\r\n", start);
            if (end != std::string::npos)
            {
                actualContentType = body.substr(start, end - start);
                // Remove any trailing whitespace
                actualContentType.erase(actualContentType.find_last_not_of(" \t\r\n") + 1);
            }
        }

        // Extract binary data
        size_t dataStart = body.find("\r\n\r\n", fileStart);
        if (dataStart == std::string::npos)
        {
            std::cerr << "No data section found" << std::endl;
            throw 400;
        }
        dataStart += 4; // Skip \r\n\r\n

        size_t dataEnd = body.find(boundary, dataStart);
        if (dataEnd != std::string::npos)
        {
            dataEnd -= 2; // Remove \r\n before boundary
        }
        else
        {
            dataEnd = body.length();
        }

        // Copy binary data
        fileData.assign(body.begin() + dataStart, body.begin() + dataEnd);
    }
    else
    {
        // Raw binary data (no multipart)
        fileData.assign(body.begin(), body.end());
        filename = "raw_upload";
    }
    bool isValidType = false;
    if (actualContentType.find("audio/") == 0 ||
        actualContentType.find("video/") == 0 ||
        actualContentType.find("image/") == 0)
    {
        isValidType = true;
    }

    // Check file extension as backup
    if (!isValidType && !filename.empty())
    {
        std::string ext = filename.substr(filename.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == "mp3" || ext == "wav" || ext == "mp4" ||
            ext == "avi" || ext == "jpg" || ext == "png")
        {
            isValidType = true;
        }
    }

    if (!isValidType)
    {
        std::cerr << "Invalid file type: " << actualContentType << std::endl;
        throw 415;
    }

    std::string timestamp = std::to_string(std::time(nullptr));
    std::string uniqueFilename = timestamp + "_" + filename;

    Req.setPath(location.getUploadPath() + uniqueFilename);

    std::ofstream file(location.getUploadPath() + uniqueFilename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening file for writing: " << location.getUploadPath() + uniqueFilename << std::endl;
        throw 500;
    }

    file.write(fileData.data(), fileData.size());
    if (!file.good())
    {
        std::cerr << "Error writing to file: " << location.getUploadPath() + uniqueFilename << std::endl;
        file.close();
        std::remove((location.getUploadPath() + uniqueFilename).c_str()); // Clean up partial file
        throw 500;
    }

    file.close();

    std::string response_body = "SUCCESS";

    std::string response =
        "HTTP/1.1 201 Created\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " +
        std::to_string(response_body.size()) + "\r\n"
                                               "\r\n" +
        response_body;

    send(fd, response.c_str(), response.size(), 0);
}

void GetResponse(Request &Req, Location &location, int fd, Server &server)
{
    if (access(Req.getPath().c_str(), F_OK) == -1 && access(Req.getPath().c_str(), R_OK) == -1)
        throw 404;
    if (Req.getPath().back() == '/')
    {
        std::string index = location.getIndex();
        if (index.empty() && location.getAutoindex() == false)
            throw 404;
        else if (index.empty() && location.getAutoindex() == true)
        {
            std::string autoIndex = "<!DOCTYPE html>\n"
                                    "<html>\n"
                                    "<head><title>My Server</title></head>\n"
                                    "<body style=\"background-color: black; color: white;\">\n"
                                    "  <h1>Autoindex!</h1>\n"
                                    "</body>\n"
                                    "</html>\n";
            std::string response =
                getResponseLine(200) +
                "Content-Length: " + std::to_string(autoIndex.size()) + "\r\n"
                                                                        "Content-Type: text/html; charset=UTF-8\r\n"
                                                                        "Connection: keep-alive\r\n"
                                                                        "\r\n" +
                autoIndex;
            send(fd, response.c_str(), response.size(), 0);
        }
        else
            Req.setPath(Req.getPath() + index);
        if (access(Req.getPath().c_str(), F_OK) == -1 && access(Req.getPath().c_str(), R_OK) == -1)
            throw 404;
    }
    sendPage(fd, 200, Req.getPath(), Req.getHeader("Connection"));
}

void Response::sendResponse(int fd, Request &Req, int statusCode, Server &server)
{
    if (statusCode >= 400 && statusCode < 600)
        sendErrorPage(fd, Req, statusCode, server);
    else
    {
        Location location = getLocation(Req.getPath(), server.getLocations());
        // if (hasRedirection(location.getRedirections()))
        // {
        //     sendRedirectionResponse(fd, location.getRedirections(), Req.getPath(), 301);
        //     return;
        // }

        
        try
        {
            if (!isAllowedMethod(Req.getMethod(), location.getAllowedMethods()))
                throw 405;
            // skip favicon.ico
            if (Req.getPath() == "/favicon.ico")
            {
                std::string response = getResponseLine(200) +
                                       "Content-Length: 0\r\n"
                                       "Content-Type: text/plain; charset=UTF-8\r\n"
                                       "Connection: keep-alive\r\n"
                                       "\r\n";
                send(fd, response.c_str(), response.size(), 0);
                return;
            }
            if (Req.getMethod() == "POST" && location.getUploadPath().empty())
                throw 403;
    
            setProperRequestPath(Req, location.getRoot(), location.getPath());
            if (Req.getMethod() == "GET")
                GetResponse(Req, location, fd, server);
            // else if (Req.getMethod() == "POST")
            //     PostResponse(Req, location, fd, server);
            else if (Req.getMethod() == "DELETE")
            {
                std::cout << Req.getPath() << std::endl;
                if (access(Req.getPath().c_str(), F_OK) == -1)
                    throw 404;
                if (remove(Req.getPath().c_str()) != 0)
                {
                    perror("remove");
                    throw 500;
                }
                std::string response_body = "File deleted successfully";
                std::string response =
                    getResponseLine(200) +
                    "Content-Length: " + std::to_string(response_body.size()) + "\r\n"
                                                                                "Content-Type: text/plain; charset=UTF-8\r\n"
                                                                                "Connection: keep-alive\r\n"
                                                                                "\r\n" +
                    response_body;
                send(fd, response.c_str(), response.size(), 0);
            }
        }
        catch (int code)
        {
            return sendErrorPage(fd, Req, code, server);
        }
    }
}
