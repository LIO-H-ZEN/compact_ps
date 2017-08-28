//
//  localenv.h
//  core
//
//  Created by Chunwei on 2/6/15.
//  Copyright (c) 2015 Chunwei. All rights reserved.
//
#ifndef LZC_UTILS_ENV_UTIL_H
#define LZC_UTILS_ENV_UTIL_H
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Common.h"

namespace lzc {
// inet addr:172.17.0.1  Bcast:0.0.0.0  Mask:255.255.0.0
std::string get_local_ip() {
  struct ifaddrs *ifAddrStruct = NULL;
  struct ifaddrs *ifa = NULL;
  void *tmpAddrPtr = NULL;
  std::string ip;

  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) {
      continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      if (strcmp(addressBuffer, "127.0.0.1") != 0) {
        ip = addressBuffer;
      }
      // printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
    }
  }
  if (ifAddrStruct != NULL)
    freeifaddrs(ifAddrStruct);
  return std::move(ip);
}

}; // namespace lzc
#endif
