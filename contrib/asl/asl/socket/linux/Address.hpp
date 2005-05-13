#ifndef ASL_SOCKET_LINUX_ADDRESS_HPP_INCLUDED
#define ASL_SOCKET_LINUX_ADDRESS_HPP_INCLUDED

#include <asl/socket/Addresses.hpp>
#include <asl/socket/Protocol.hpp>

#include <netdb.h>

#include <vector>
#include <string>

namespace asl
{

typedef int AddressType;

class Address
{
private:
  hostent* hostInfo;
  int portNumber;

  std::vector<std::string> fillVector(char** list) const;

public:
  Address(char const* addr, int port, AddressType type=AF_INET);

  char const* address() const;

  char const* host() const;

  int port() const;

  AddressType type() const;

  std::vector<std::string> allAddresses() const;

  std::vector<std::string> allHosts() const;
};
}

#endif

