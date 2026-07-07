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
		std::vector<ServerContext>* GetServers() const;
		ServerContext* GetServer(size_t index);
		ServerContext* GetLastServer();
};