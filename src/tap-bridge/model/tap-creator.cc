/*
 * Copyright (c) 2009 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/creator-utils.h"
#include "ns3/encode-decode.h"
#include "ns3/mac48-address.h"

#include <cerrno>
#include <cstdlib>
#include <cstring> // for strerror
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <linux/if_tun.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sstream>
#include <stdint.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define TAP_MAGIC 95549

static bool gVerbose = false; // Set to true to turn on logging messages.

static sockaddr
CreateInetAddress(uint32_t networkOrder)
{
    union {
        struct sockaddr any_socket;
        struct sockaddr_in si;
    } s;

    s.si.sin_family = AF_INET;
    s.si.sin_port = 0; // unused
    s.si.sin_addr.s_addr = htonl(networkOrder);
    return s.any_socket;
}

static int
CreateTap(const char* dev, const char* ip, const char* mac, const char* mode, const char* netmask)
{
    //
    // Creation and management of Tap devices is done via the tun device
    //
    int tap = open("/dev/net/tun", O_RDWR);
    ABORT_IF(tap == -1, "Could not open /dev/net/tun", true);

    //
    // Allocate a tap device, making sure that it will not send the tun_pi header.
    // If we provide a null name to the ifr.ifr_name, we tell the kernel to pick
    // a name for us (i.e., tapn where n = 0..255.
    //
    // If the device does not already exist, the system will create one.
    //
    struct ifreq ifr;
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strcpy(ifr.ifr_name, dev);
    int status = ioctl(tap, TUNSETIFF, (void*)&ifr);
    ABORT_IF(status == -1, "Could not allocate tap device", true);

    std::string tapDeviceName = (char*)ifr.ifr_name;
    LOG("Allocated TAP device " << tapDeviceName);

    //
    // Operating mode "2" corresponds to USE_LOCAL and "3" to USE_BRIDGE mode.
    // This means that we expect that the user will have named, created and
    // configured a network tap that we are just going to use.  So don't mess
    // up his hard work by changing anything, just return the tap fd.
    //
    if (std::string(mode) == "2" || std::string(mode) == "3")
    {
        LOG("Returning precreated tap ");
        return tap;
    }

    //
    // Set the hardware (MAC) address of the new device
    //
    ifr.ifr_hwaddr.sa_family = 1; // this is ARPHRD_ETHER from if_arp.h
    ns3::Mac48Address(mac).CopyTo((uint8_t*)ifr.ifr_hwaddr.sa_data);
    status = ioctl(tap, SIOCSIFHWADDR, &ifr);
    ABORT_IF(status == -1, "Could not set MAC address", true);
    LOG("Set device MAC address to " << mac);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    //
    // Bring the interface up.
    //
    status = ioctl(fd, SIOCGIFFLAGS, &ifr);
    ABORT_IF(status == -1, "Could not get flags for interface", true);
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    status = ioctl(fd, SIOCSIFFLAGS, &ifr);
    ABORT_IF(status == -1, "Could not bring interface up", true);
    LOG("Device is up");

    //
    // Set the IP address of the new interface/device.
    //
    ifr.ifr_addr = CreateInetAddress(ns3::Ipv4Address(ip).Get());
    status = ioctl(fd, SIOCSIFADDR, &ifr);
    ABORT_IF(status == -1, "Could not set IP address", true);
    LOG("Set device IP address to " << ip);

    //
    // Set the net mask of the new interface/device
    //
    ifr.ifr_netmask = CreateInetAddress(ns3::Ipv4Mask(netmask).Get());
    status = ioctl(fd, SIOCSIFNETMASK, &ifr);
    ABORT_IF(status == -1, "Could not set net mask", true);
    LOG("Set device Net Mask to " << netmask);

    return tap;
}

int
main(int argc, char* argv[])
{
    int c;
    char* dev = (char*)"";
    char* ip = nullptr;
    char* mac = nullptr;
    char* netmask = nullptr;
    char* operatingMode = nullptr;
    char* path = nullptr;

    opterr = 0;

    while ((c = getopt(argc, argv, "vd:i:m:n:o:p:")) != -1)
    {
        switch (c)
        {
        case 'd':
            dev = optarg; // name of the new tap device
            break;
        case 'i':
            ip = optarg; // ip address of the new device
            break;
        case 'm':
            mac = optarg; // mac address of the new device
            break;
        case 'n':
            netmask = optarg; // net mask for the new device
            break;
        case 'o':
            operatingMode = optarg; // operating mode of tap bridge
            break;
        case 'p':
            path = optarg; // path back to the tap bridge
            break;
        case 'v':
            gVerbose = true;
            break;
        }
    }

    //
    // We have got to be able to coordinate the name of the tap device we are
    // going to create and or open with the device that an external Linux host
    // will use.  If this name is provided we use it.  If not we let the system
    // create the device for us.  This name is given in dev
    //
    LOG("Provided Device Name is \"" << dev << "\"");

    //
    // We have got to be able to assign an IP address to the tap device we are
    // allocating.  This address is allocated in the simulation and assigned to
    // the tap bridge.  This address is given in ip.
    //
    ABORT_IF(ip == nullptr, "IP Address is a required argument", 0);
    LOG("Provided IP Address is \"" << ip << "\"");

    //
    // We have got to be able to assign a Mac address to the tap device we are
    // allocating.  This address is allocated in the simulation and assigned to
    // the bridged device.  This allows packets addressed to the bridged device
    // to appear in the Linux host as if they were received there.
    //
    ABORT_IF(mac == nullptr, "MAC Address is a required argument", 0);
    LOG("Provided MAC Address is \"" << mac << "\"");

    //
    // We have got to be able to assign a net mask to the tap device we are
    // allocating.  This mask is allocated in the simulation and given to
    // the bridged device.
    //
    ABORT_IF(netmask == nullptr, "Net Mask is a required argument", 0);
    LOG("Provided Net Mask is \"" << netmask << "\"");

    //
    // We have got to know whether or not to create the TAP.
    //
    ABORT_IF(operatingMode == nullptr, "Operating Mode is a required argument", 0);
    LOG("Provided Operating Mode is \"" << operatingMode << "\"");

    //
    // This program is spawned by a tap bridge running in a simulation.  It
    // wants to create a socket as described below.  We are going to do the
    // work here since we're running suid root.  Once we create the socket,
    // we have to send it back to the tap bridge.  We do that over a Unix
    // (local interprocess) socket.  The tap bridge created a socket to
    // listen for our response on, and it is expected to have encoded the address
    // information as a string and to have passed that string as an argument to
    // us.  We see it here as the "path" string.  We can't do anything useful
    // unless we have that string.
    //
    ABORT_IF(path == nullptr, "path is a required argument", 0);
    LOG("Provided path is \"" << path << "\"");

    //
    // The whole reason for all of the hoops we went through to call out to this
    // program will pay off here.  We created this program to run as suid root
    // in order to keep the main simulation program from having to be run with
    // root privileges.  We need root privileges to be able to futz with the
    // Tap device underlying all of this.  So all of these hoops are to allow
    // us to execute the following code:
    //
    LOG("Creating Tap");
    int sock = CreateTap(dev, ip, mac, operatingMode, netmask);
    ABORT_IF(sock == -1, "main(): Unable to create tap socket", 1);

    //
    // Send the socket back to the tap net device so it can go about its business
    //
    ns3::SendSocket(path, sock, TAP_MAGIC);

    return 0;
}
