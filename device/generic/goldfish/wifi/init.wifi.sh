#!/vendor/bin/sh

# Do all the setup required for WiFi.
# The kernel driver mac80211_hwsim has already created two virtual wifi devices
# us. These devices are connected so that everything that's sent on one device
# is recieved on the other and vice versa. This allows us to create a fake
# WiFi network with an access point running inside the guest. Here is the setup
# for that and the basics of how it works.
#
# Create a namespace named router and move eth0 to it. Create a virtual ethernet
# pair of devices and move both one virtual ethernet interface and one virtual
# wifi interface into the router namespace. Then set up NAT networking for those
# interfaces so that traffic flowing through them reach eth0 and eventually the
# host and the internet. The main network namespace will now only see the other
# ends of those pipes and send traffic on them depending on if WiFi or radio is
# used.  Finally run hostapd in the network namespace to create an access point
# for the guest to connect to and dnsmasq to serve as a DHCP server for the WiFi
# connection.
#
#          main namespace                     router namespace
#       -------       ----------   |    ---------------
#       | ril |<----->| radio0 |<--+--->| radio0-peer |<-------+
#       -------       ----------   |    ---------------        |
#                                  |            ^              |
#                                  |            |              |
#                                  |            v              v
#                                  |      *************     --------
#                                  |      * ipv6proxy *<--->| eth0 |<--+
#                                  |      *************     --------   |
#                                  |            ^              ^       |
#                                  |            |              |       |
#                                  |            v              |       |
# ------------------   ---------   |        ---------          |       |
# | wpa_supplicant |<->| wlan0 |<--+------->| wlan1 |<---------+       |
# ------------------   ---------   |        ---------                  |
#                                  |         ^     ^                   |
#                                  |         |     |                   v
#                                  |         v     v                --------
#                                  | ***********  ***********       | host |
#                                  | * hostapd *  * dnsmasq *       --------
#                                  | ***********  ***********
#

NAMESPACE="router"
rm -rf /data/vendor/var/run/netns/${NAMESPACE}
rm -rf /data/vendor/var/run/netns/${NAMESPACE}.pid
# Lower the MTU of the WiFi interface to prevent issues with packet injection.
# The MTU of the WiFi monitor interface cannot be higher than 1500 but injection
# requires extra space for injection headers which count against the MTU. So if
# a 1500 byte payload needs to be injected it will fail because with the
# additional headers the total amount of data will exceed 1500 bytes. This way
# the payload is restricted to a smaller size that should leave room for the
# injection headers.
/system/bin/ip link set wlan0 mtu 1400

createns ${NAMESPACE}

# If this is a clean boot we need to copy the hostapd configuration file to the
# data partition where netmgr can change it if needed. If it already exists we
# need to preserve the existing settings.
if [ ! -f /data/vendor/wifi/hostapd/hostapd.conf ]; then
    cp /vendor/etc/simulated_hostapd.conf /data/vendor/wifi/hostapd/hostapd.conf
    chown wifi:wifi /data/vendor/wifi/hostapd/hostapd.conf
    chmod 660 /data/vendor/wifi/hostapd/hostapd.conf
fi

# createns will have created a file that contains the process id (pid) of a
# process running in the network namespace. This pid is needed for some commands
# to access the namespace.
PID=$(cat /data/vendor/var/run/netns/${NAMESPACE}.pid)

# Move the WiFi monitor interface to the other namespace and bring it up. This
# is what we use for injecting WiFi frames from the outside world.
/system/bin/ip link set hwsim0 netns ${PID}
execns ${NAMESPACE} /system/bin/ip link set hwsim0 up

# Start the network manager as soon as possible after the namespace is available.
# This ensures that anything that follows is properly managed and monitored.
setprop ctl.start netmgr

/system/bin/ip link set eth0 netns ${PID}
/system/bin/ip link add radio0 type veth peer name radio0-peer
/system/bin/ip link set radio0-peer netns ${PID}
# Enable privacy addresses for radio0, this is done by the framework for wlan0
sysctl -wq net.ipv6.conf.radio0.use_tempaddr=2
/system/bin/ip addr add 192.168.200.2/24 broadcast 192.168.200.255 dev radio0
execns ${NAMESPACE} /system/bin/ip addr add 192.168.200.1/24 dev radio0-peer
execns ${NAMESPACE} sysctl -wq net.ipv6.conf.all.forwarding=1
execns ${NAMESPACE} /system/bin/ip link set radio0-peer up
# Start the dhcp client for eth0 to acquire an address
setprop ctl.start dhcpclient_rtr
# Create iptables entries. -w will cause an indefinite wait for the exclusive
# lock. Without this flag iptables can sporadically fail if something else is
# modifying the iptables at the same time. -W indicates the number of micro-
# seconds between each retry. The default is one second which seems like a long
# time. Keep this short so we don't slow down startup too much.
execns ${NAMESPACE} /system/bin/iptables -w -W 50000 -t nat -A POSTROUTING -s 192.168.232.0/21 -o eth0 -j MASQUERADE
execns ${NAMESPACE} /system/bin/iptables -w -W 50000 -t nat -A POSTROUTING -s 192.168.200.0/24 -o eth0 -j MASQUERADE
/vendor/bin/iw phy phy1 set netns $PID

execns ${NAMESPACE} /system/bin/ip addr add 192.168.232.1/21 dev wlan1
execns ${NAMESPACE} /system/bin/ip link set wlan1 mtu 1400
execns ${NAMESPACE} /system/bin/ip link set wlan1 up
# Start the IPv6 proxy that will enable use of IPv6 in the main namespace
setprop ctl.start ipv6proxy
execns ${NAMESPACE} sysctl -wq net.ipv4.ip_forward=1
# Start hostapd, the access point software
setprop ctl.start emu_hostapd
# Start DHCP server for the wifi interface
setprop ctl.start dhcpserver
/system/bin/ip link set radio0 up
