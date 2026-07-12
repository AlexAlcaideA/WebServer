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
		std::ostream& operator<<(std::ostream& os) const;
		void AddServer(const ServerContext& server);

		const std::vector<ServerContext>* GetServers() const;
		const ServerContext* GetServer(size_t index) const;
		const ServerContext* GetLastServer() const;
};