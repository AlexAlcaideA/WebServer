#pragma once

#include "ConfigContext.hpp"
#include "ServerContext.hpp"

class GlobalContext : public ConfigContext
{
	private:
		std::vector<ServerContext>* _server;				// Multiple,	Duplicates,		No optional,	Default: -
	public:
		GlobalContext();
		GlobalContext(const GlobalContext& other);
		GlobalContext& operator=(const GlobalContext& other);
		~GlobalContext();
		void AddServer(const ServerContext& server);

		const std::vector<ServerContext>* GetServers() const;
		const ServerContext* GetServer(size_t index) const;
		ServerContext* GetLastServer();
		const ServerContext* GetLastServer() const;
};

std::ostream& operator<<(std::ostream& os, const GlobalContext& other);
