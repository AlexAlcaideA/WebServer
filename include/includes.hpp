#ifndef INCLUDES_HPP
# define INCLUDES_HPP
//Standard
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <csignal>

#include <algorithm>

#include <vector>
#include <set>
#include <map>
#include <stdexcept>
#include <ostream>
//Propios
#include "utils/HttpMethod.hpp"
#include "utils/HttpHeaders.hpp"
#include "utils/HttpStatus.hpp"
#include "utils/StringUtils.hpp"

#include "defines.h"
#include "defaultresponse.hpp"

extern volatile sig_atomic_t g_running;

#endif
