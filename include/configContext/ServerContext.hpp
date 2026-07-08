#pragma once

#include "ConfigContext.hpp"
#include "LocationContext.hpp"
#include <set>

class ServerContext : public ConfigContext
{
	private:
		struct ServerListen
		{
			std::string serverIp;
			unsigned int port;
			bool operator<(const ServerListen& other) const
			{
				if (serverIp != other.serverIp)
					return serverIp < other.serverIp;
				return port < other.port;
			}
			bool operator==(const ServerListen& other) const
			{
				return serverIp == other.serverIp && port == other.port;
			}
		};
		std::set<ServerListen>* _listen;					// Multiple,	No duplicates,	Optional,		Default: INADDR_ANY:http
		std::set<std::string>* _serverName;					// Multiple,	No Duplicates,	Optional,		Default: ""
		std::map<std::string, LocationContext>* _location;	// Multiple,	No duplicates,	Optional,		Default: /
		std::map<std::string, std::string>* _cgiHandlers;	// Multiple,	No duplicates,	Optional,		Default: -
	public:
		ServerContext();
		ServerContext(const ServerContext& other);
		ServerContext& operator=(const ServerContext& other);
		~ServerContext();
		bool operator==(const ServerContext& other) const;
		std::ostream& operator<<(std::ostream& os) const;
		void SetListen(const unsigned int& port);
		void SetListen(const std::string& ip, const unsigned int& port);
		void AddServerName(const std::string& name);
		void AddLocation(const LocationContext& location);
		void SetCgiHandler(const std::string& ext, const std::string& interp);
		std::map<std::string, LocationContext>* GetLocations() const;
		LocationContext* GetLocation(size_t index);
		LocationContext* GetLastLocation();
};