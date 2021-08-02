#!system/bin/sh
#Add iptables rules needed by vendor
iptables -w -t filter -F oem_in;
iptables -w -t filter -X oem_in;
iptables -w -t filter -N oem_in;
iptables -w -t filter -A INPUT -j oem_in;

ip6tables -w -t filter -F oem_in;
ip6tables -w -t filter -X oem_in;
ip6tables -w -t filter -N oem_in;
ip6tables -w -t filter -A INPUT -j oem_in;

iptables -w -t filter -F oem_mobile;
iptables -w -t filter -X oem_mobile;
iptables -w -t filter -N oem_mobile;
iptables -w -t filter -A  oem_out -o ppp+ -j oem_mobile;
iptables -w -t filter -A  oem_out -o ccmni+ -j oem_mobile;
iptables -w -t filter -A  oem_out -o ccemni+ -j oem_mobile;
iptables -w -t filter -A  oem_out -o usb+ -j oem_mobile;
iptables -w -t filter -A  oem_out -o cc2mni+ -j oem_mobile;

ip6tables -w -t filter -F oem_mobile;
ip6tables -w -t filter -X oem_mobile;
ip6tables -w -t filter -N oem_mobile;
ip6tables -w -t filter -A  oem_out -o ppp+ -j oem_mobile;
ip6tables -w -t filter -A  oem_out -o ccmni+ -j oem_mobile;
ip6tables -w -t filter -A  oem_out -o ccemni+ -j oem_mobile;
ip6tables -w -t filter -A  oem_out -o usb+ -j oem_mobile;
ip6tables -w -t filter -A  oem_out -o cc2mni+ -j oem_mobile;

iptables -w -t filter -F oem_wifi;
iptables -w -t filter -X oem_wifi;
iptables -w -t filter -A oem_out -o wlan+ -j oem_wifi;

ip6tables -w -t filter -F oem_wifi;
ip6tables -w -t filter -X oem_wifi;
ip6tables -w -t filter -A oem_out -o wlan+ -j oem_wifi;

iptables -w -t filter -F oem_data;
iptables -w -t filter -X oem_data;
iptables -w -t filter -N oem_data;
iptables -w -t filter -A  oem_out -j oem_data;

ip6tables -w -t filter -F oem_data;
ip6tables -w -t filter -X oem_data;
ip6tables -w -t filter -N oem_data;
ip6tables -w -t filter -A oem_out -j oem_data;

iptables -w -t filter -F oem_priou;
iptables -w -t filter -X oem_priou;
iptables -w -t filter -N oem_priou;
iptables -w -t filter -A oem_data -j oem_priou;

ip6tables -w -t filter -F oem_priou;
ip6tables -w -t filter -X oem_priou;
ip6tables -w -t filter -N oem_priou;
ip6tables -w -t filter -A oem_data -j oem_priou;

iptables -w -t filter -F oem_priot;
iptables -w -t filter -X oem_priot;
iptables -w -t filter -N oem_priot;
iptables -w -t filter -A oem_data -j oem_priot ;

ip6tables -w -t filter -F oem_priot;
ip6tables -w -t filter -X oem_priot;
ip6tables -w -t filter -N oem_priot;
ip6tables -w -t filter -A oem_data -j oem_priot ;


