#include "httprequest.hpp"

httpreq::httpreq(std::string text)
{
        size_t first = text.find(' ');
        /*if (first == std::string::npos)//ultimo espacio
        {
            close(client_fd);
            continue;
        }*/
        method = text.substr(0, first);
        size_t second = text.find(' ', first + 1);
        /*if (second == std::string::npos)
        {
            close(client_fd);
            continue;
        }*/
        path = text.substr(first + 1, second - first - 1);

        // Evitamos acceder a "/" directamente
        if (path == "/" || path == "..")
            path = "/index.html";
}
const std::string httpreq::getMethod(void) const
{
    return method;
}

const std::string httpreq::getPath(void) const
{
    return path;
}

const std::string httpreq::getVersion(void) const
{
    return version;
}

const std::string httpreq::getHost(void) const
{
    return host;
}

const std::string httpreq::getAgent(void) const
{
    return agent;
}

const std::string httpreq::getAccept(void) const
{
    return accept;
}const std::string httpreq::getLenght(void) const
{
    return lenght;
}
