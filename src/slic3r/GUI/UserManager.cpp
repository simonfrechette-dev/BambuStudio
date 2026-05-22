// Qt6 stub for UserManager.cpp
#include "UserManager.hpp"
// All implementations are stubs pending Qt port.

namespace Slic3r {

UserManager::UserManager(NetworkAgent* agent) : m_agent(agent) {}
UserManager::~UserManager() {}
void UserManager::set_agent(NetworkAgent* agent) { m_agent = agent; }
int  UserManager::parse_json(std::string /*payload*/) { return 0; }

} // namespace Slic3r
