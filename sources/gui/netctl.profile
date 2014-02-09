! - обязательное поле
[ddsg] - условие для этой опции
в массиве первая опция - дефолт
yes - бинарная опция, по дефолту стоит no (и наоборот)

GENERAL
=======
Description=
! Connection=ethernet|wireless|bond|bridge|dummy|pppoe|mobile_ppp|tunnel|tuntap|vlan
! Interface=
BindsToInterfaces=(массив интерфейсов, которые будет ожидать этот профиль)
After=(список профилей, которые будут запущены перед этим)
ExecUpPost=команда, которая будет запущена после
ExecDownPre=команда, которая будет запущена перед
ForceConnect=yes - включить, даже если интерфейс уже поднят

IP
==
! IP=static|dhcp|no
! IP6=static|stateless|dhcp-noaddr|dhcp|no
[IP=static] Address=('192.168.1.23/24' '192.168.1.87/24')
[IP=static] Gateway='192.168.1.1'
Routes=('192.168.0.0/24 via 192.168.1.2')
[IP6=static]|[IP6=stateless] Address6=('1234:5678:9abc:def::1/64' '1234:3456::123/96')
[IP6=static]|[IP6=stateless] Gateway6='1234:0:123::abcd'
Routes6=('192.168.0.0/24 via 192.168.1.2')
[IP=dhcp] DHCPClient=dhcpcd|dhclient
[IP=dhcp] DHCPReleaseOnStop=yes - release the DHCP lease when the profile is stopped.
[IP=dhcp][DHCPClient=dhcpcd] DhcpcdOptions=опции
[IP=dhcp][DHCPClient=dhclient] DhclientOptions=опции
[IP=dhcp][DHCPClient=dhclient] DhclientOptions6=опции
IPCustom=('addr add dev eth0 192.168.1.23/24 brd +' 'route add default via 192.168.1.1') - команда для ip напрямую
Hostname=
DNS=('192.168.1.1')
DNSDomain= domain для /etc/resolv.conf
DNSSearch= search для /etc/resolv.conf
DNSOptions=() массив options для /etc/resolv.conf
TimeoutDHCP=30 - время в секундах
TimeoutDAD=3 - время в секундах для ipv6 удаление дубликата

ETHERNET
========
SkipNoCarrier=yes
Auth8021X=yes
WPAConfigFile=/etc/wpa_supplicant.conf
WPADriver=wired|nl80211|wext
TimeoutCarrier=5
TimeoutWPA=15
+ IP

WIRELESS
========
Security=none|wep|wpa|wpa-configsection|wpa-config
! ESSID=
AP=MAC address
[Security=wep]|[Security=wpa] ! Key='superpass'
Hidden=yes
AdHoc=yes
ScanFrequencies=A space-separated list of frequencies in MHz to scan when searching for the network.
Priority=0
[Security=wpa-configsection] ! WPAConfigSection=(
    'ssid="University"'
    'key_mgmt=WPA-EAP'
    'eap=TTLS'
    'group=TKIP'
    'pairwise=TKIP CCMP'
    'anonymous_identity="anonymous"'
    'identity="myusername"'
    'password="mypassword"'
    'priority=1'
    'phase2="auth=PAP"'
)
WPAConfigFile=/etc/wpa_supplicant.conf
Country=страна
WPAGroup=wheel (группа)
WPADriver=nl80211,wext
TimeoutWPA=15
RFKill=for X in 0..2 $(cat /sys/class/rfkill/rfkillX/name)|auto
ExcludeAuto=yes
+ IP

BOND
====
! BindsToInterfaces=
+ IP

BRIDGE
======
! BindsToInterfaces=
+ IP

DUMMY
=====
+ IP

PPPOE
=====
-User='example@yourprovider.com'
-Password='very secret'
ConnectionMode=persist|demand
[ConnectionMode=demand] IdleTimeout=0
MaxFail=5
DefaultRoute=false
UsePeerDNS=false
PPPUnit=ppp0|ppp1|...
-LCPEchoInterval=30
-LCPEchoFailure=4
OptionsFile=доп файл
PPPoEService=имя
PPPoEAC=имя
PPPoESession='sessid:macaddr'
PPPoEMAC=мак
PPPoEIP6=yes

MOBILE_PPP
==========
MaxFail=5
DefaultRoute=false
UsePeerDNS=false
-User='example@yourprovider.com'
-Password='very secret'
AccessPointName=имя
Pin=код
Mode=None|3Gpref|3Gonly|GPRSpref|GPRSonly
OptionsFile=доп файл

TUNNEL
======
Mode=ipip|gre|sit|isatap|ip6ip6|ipip6|ip6gre|any
Local='172.16.0.1'
Remote='216.66.80.26'
+ ip

TUNTAP
======
Mode=tun|tap
User='nobody'
Group='nobody'
+ ip

VLAN
====
! BindsToInterfaces=
VLANID=55
