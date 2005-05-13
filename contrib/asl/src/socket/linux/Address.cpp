#include <asl/socket/Address.hpp>
#include <asl/socket/SocketException.hpp>
#include <asl/socket/Addresses.hpp>
#include <asl/socket/Protocol.hpp>

#include <asl/assert.hpp>

#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <vector>
#include <string>

namespace asl
{

std::vector<std::string> Address::fillVector(char** list) const
{
  std::vector<std::string> vec;

  for(int i=0; list[i]; ++i)
  {
    vec.push_back(list[i]);
  }

  return vec;
}

Address::Address(char const* addr, int port, AddressType type)
: portNumber(htons(port))
{
  ASL_ASSERT(addr, "char const* addr mustn't be a NULL pointer");
  hostInfo=gethostbyname(addr);
  if(!hostInfo)
  {
    in_addr_t ip;
    if(inet_pton(type, addr, &ip)!=1)
      throw ASL_SOCKET_EXCEPTION("inet_pton failed");

    hostInfo=gethostbyaddr((char*)&ip, sizeof(ip), type);
    if(!hostInfo)
      throw ASL_SOCKET_EXCEPTION("gethostbyaddr and gethostbyname failed");
  }
}

char const* Address::address() const
{
  return hostInfo->h_name;
}

char const* Address::host() const
{
  return hostInfo->h_addr;
}

int Address::port() const
{
  return portNumber;
}

AddressType Address::type() const
{
  return hostInfo->h_addrtype;
}

std::vector<std::string> Address::allAddresses() const
{
  return fillVector(hostInfo->h_aliases);
}

std::vector<std::string> Address::allHosts() const
{
  return fillVector(hostInfo->h_addr_list);
}

}

