# Bluefiled(BF) on Linux

In this document, we explain how to install Linux distributions on Bluefield. In our machine, we have the host CPU and the Arm of BF. We need to install the software on both the host and the BF side. 

Firstly, we will install the OFED and rshim drivers in the host. [Mellanox OFED - is a Mellanox tested and packaged version of OFED and supports two interconnect types using the same RDMA (remote DMA) and kernel bypass APIs called OFED verbs – InfiniBand and Ethernet.] 
Secondly, we will install OFED + Ubuntu in the BF. 
Thirdly, we will set the configuration of the BF (Separated mode vs Embedded mode).
Fourthly, we will test our OFED with one RDMA example and another with OFED examples. (local and remote rdma) 
Finally, we will create NAT; BF has its private network to the host via the USB connection, you can also move packages from host to BF via scp. In order to have network enablement inside the BF we have two options 1) Bridge, to give the BF an external ip address 2)SNAT and DNAT (source and destination NAT) via setting up the routing on the host. 


Notes: 

1) In order to configure two remote machine with rdma, you need to have different ips for the interface two outgoing network interfacs. 
For example use for one machine, enp133s0f0 192.168.0.20/24 enp133s0f1 192.168.0.21/24 
and for the another ens2f0 192.168.0.22/24 ens2f1 192.168.0.23/24 
2) if the two outgoing network interfacs are not UP (after the configuration) check that your cables are connected correctly to the switch. Additionally, a power outage may reset switch configuration especially for a split cable.
See the switch configutation**. 
3) enabling the external network may cause failures if you use docker, etc. run sudo service docker restart to update everything..


# Bluefiled on Ubuntu 18.04
## Requirment 
1) Host system is Ubuntu 18.04
2) BlueField NIC inserted in a host PCIe slot
3) USB cable connecting the NIC card and the host

## preinstall

Go to:
https://www.mellanox.com/products/software/bluefield
* Download (to host machine) MLNX_OFED. <mlnx_ofed>

![2](https://user-images.githubusercontent.com/28096724/87292707-e2487b80-c509-11ea-9263-7e3f5e8b48a1.png)

* Download BlueField BlueOS. <BlueOS_dir>
* Download (to host machine) BlueField Ubuntu Server 18.04 image. <bf_img.bfb>

![1](https://user-images.githubusercontent.com/28096724/87292641-c6dd7080-c509-11ea-8031-f21ee3f094da.png)


Run:

	sudo apt install linux-signed-generic-hwe-18.04
	sudo reboot --force
	sudo apt install build-essential debhelper autotools-dev dkms
	
## In the Host:

### OFED: 
	mount <mlnx_ofed>.iso /mnt
	sudo /mnt/uninstall.sh --force
	sudo /mnt/mlnxofedinstall --add-kernel-support 
You should see that it is installed: 
Querying Mellanox devices firmware ...

Check your firmware version:
	
	sudo mlxfwmanage

### rshim drivers: 
	cd <BlueOS_dir>/src/drivers/rshim
	sudo modprobe -vr rshim_pcie
	sudo modprobe -vr rshim_net
	sudo modprobe -vr rshim_usb

##### First installation option: 

	dpkg-buildpackage -us -uc -nc
	sudo dpkg -i ../rshim-dkms_*.deb
	sudo modprobe rshim_usb
	sudo modprobe rshim_net
	
##### If the "First" did not work, pease  install it manually by:

	make -C /lib/modules/`uname -r`/build M=$PWD
	sudo modprobe rshim_usb
	sudo modprobe rshim_net
	
	vim /etc/udev/rules.d/91-tmfifo_net.rules
Add this line to tmfifo_net.rules:

SUBSYSTEM=="net", ACTION=="add", ATTR{address}=="00:1a:ca:ff:ff:02", ATTR{type}=="1", NAME="tmfifo_net0", RUN+="/usr/sbin/ifup tmfifo_net0"
	
	reboot --force
	ip add
	
You should be able to see interface tmfifo_net and two outgoing network interfacs UP.
	
	sudo ifconfig tmfifo_net0 192.168.100.1/24 up
This will make the 3 index GID available :
	sudo ifconfig <first_interface> 192.168.0.20 up  
	sudo ifconfig <second_interface> 192.168.0.21 up
	
![12](https://user-images.githubusercontent.com/28096724/87291069-80871200-c507-11ea-8289-3cb2709d9a44.png)



## In the BF:

### Install the image to Bluefiled (Ubuntu 18.04 and OFED):
     cat  <bf_img.bfb> > /dev/rshim0/boot
     
Username: ssh ubuntu@192.168.100.2

Password: ubuntu
### if you have a problem with ssh (or you want to change the defualt ip of tmfifo_net0) , you can enter the console 
	sudo screen /dev/rshim0/console

## Configuration:
The firmware default mode is Embedded, in order to use RoCE you should switch to Separated mode.
More information:
https://community.mellanox.com/s/article/BlueField-SmartNIC-Modes

In your host machine:

	mst start 
	mlxconfig -d /dev/mst/mt41682_pciconf0 s INTERNAL_CPU_MODEL=0
	reboot --force
	mst start 
	mlxconfig -d /dev/mst/mt41682_pciconf0 q | grep -i model
	
## Tests: (In the same machine)
	show_gids mlx5_1 or mlx5_0 
use this index GID (RoCE doesn't work with LID-based routing), by picking any index from the GID table (it should probably be the same GID on both sides).
My GID index is 3. Change it in the RDMA example in both client and server.


	cat /sys/class/infiniband/mlx5_0/ports/1/gid_attrs/ndevs/
https://community.mellanox.com/s/article/howto-configure-roce-on-connectx-4

	ip add 
Make sure that both ports are UP. if not, use the one that is up in the code. 
In client.cpp and server.cpp, update the idx of the device_list. 
 		
	struct ibv_context *context = ibv_open_device(device_list[idx]);

### Try the RDMA example: (The example doesn't require to have the two interface ports UP with IPs, because we use ROCE and ROCE has a default GID that is configured with MAC address of the port, so it can be used without an IP address (at least locally). This GID is similar to the link local IP address). 
	git clone https://github.com/LinaMaudlej/BF-linux.git
	cd rdma-RoCE-local-machine/
	cd server 
	./server_rdma 
	cd client
	./client_rdma <port number>
	
![13](https://user-images.githubusercontent.com/28096724/87291927-be386a80-c508-11ea-9da7-b78d66f2a374.png)

### Test by ibv_rc_pingpong and ibv_read_lat:
--ibv_rc_pingpong (if it doesn't work try with mlx5_0 and  192.168.0.20)
	
	ibv_rc_pingpong -d mlx5_1 -g <gid_indx> 
	ibv_rc_pingpong -g <gid_index> 192.168.0.21
	
-- ibv_read_lat

	ib_read_lat -a 
	ib_read_lat localhost -a 

## Tests: (In the remote machines)
two machines, let's assume server machine has tmfifo_net0 192.168.100.1/24 and client machine tmfifo_net0 192.168.100.1/24. Your ip of the server is <ip>)

update the line in client.cpp with the correct ip address. (it is hardcoded)

	server_addr.sin_addr.s_addr = inet_addr(ip_addr); 

### Test by ib_read_bw:
-- ib_read_bw, in server side run:

	ib_read_bw 
in client side run:

	ib_read_bw <ip>
### Try the RDMA example:
	git clone https://github.com/LinaMaudlej/BF-linux.git
	cd rdma-RoCE-remote_machines/
	cd server
	./server_rdma 
	cd client_rdma
	./client_rdma <port number>

## Enable NAT in BF

- In your host machine, show your DNS ip:
		
		systemd-resolve --status
### DNS configuration for BF	
- Go to BF and add to  /etc/resolv.conf
		
		vim /etc/resolv.conf
nameserver <DNS_ip>
nameserver 8.8.8.8
### tmfifo_net0 NIC over PCIe configuration on BF
		vim /etc/network/interfaces.d/tmfifo

auto tmfifo_net0
iface tmfifo_net0 inet static
        address 192.168.100.2/30
        gateway 192.168.100.1
        dns-nameservers <DNS_ip>
	
		sudo ifdown tmfifo_net0 && sudo ifup tmfifo_net0
you can reboot the BF to make sure the tmfifo_net0 has correctly changed
## Enable NAT in host
Activate IP-forwarding in the kernel.
	
	echo "1" > /proc/sys/net/ipv4/ip_forward
	
Allow established connections from the public interface.

	iptables -A INPUT -i <outgoing interface> -m state --state ESTABLISHED,RELATED -j ACCEPT
	
Set up IP FORWARDing and Masquerading	
	 
	 iptables --table nat --append POSTROUTING --out-interface <outgoing interface> -j MASQUERADE
	 iptables --append FORWARD --in-interface tmfifo_net0 -j ACCEPT

Allow outgoing connections
	   
	   iptables -A OUTPUT -j ACCEPT
	   iptables -A FORWARD -i <outgoing interface> -o tmfifo_net0 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

Example:
	   
sudo iptables -A FORWARD -i enp0s31f6 -o tmfifo_net0 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

sudo iptables -A FORWARD -i tmfifo_net0 -j ACCEPT

sudo iptables -t nat -A POSTROUTING -o enp0s31f6 -j MASQUERAD	
	
# Bluefiled on Centos 
TBD

# Switch configuration 
Let's take for example a switch, its output is 40GB. We want to split it to 4 cables. Every cable will be divided into 40/4=10GB (note that the BF can work with 25GB for each one). 
We are working with two machines , in order to configure it again to work with 4 cables we do the following:
1) $ssh to the machine that is connected to the switch
2) $sudo screen /dev/ttyS0 115200

-Note that, the physical order of the splitted cables doesn't matter. The switch uses MAC learning to dynamically build a forwarding database, translating a MAC address to its port. 

Find your switch interface number from the User Manual. For example, in my switch this is the manual https://www.mellanox.com/related-docs/prod_management_software/MLNX-OS_ETH_v3_6_3508_UM.pdf and I see that I am using ethernet 5.
![image](https://user-images.githubusercontent.com/28096724/89739641-0a84b500-da8b-11ea-952e-70719e7e1394.png)


3) $show interfaces ethernet 1/5
4) $enable
5) $configure terminal
6) $interface ethernet 1/5 shutdown 
7) $interface ethernet 1/5 module-type qsfp-split-4 force



## Issues
1) mlx5_core 0000:04:00.1: port_module:247:(pid 0): Port module event[error]: module 1, Cable error, Bus stuck (I2C or data shorted)

This issue happened for me and I solved it with these techniques: 
1) First, disconnect the USB port which is connected to the BF and the host machine.
2) Plug out the InfiniBand cables.
2) I reinstalled the latest OFED with the latest firmware, check the firmware compatibility with your Part Number here https://docs.mellanox.com/m/view-rendered-page.action?abstractPageId=25139410.


		mlxfwmanager 
take the Part Number. For example: MBF1M332A 
![image (1)](https://user-images.githubusercontent.com/28096724/89739497-c6dd7b80-da89-11ea-957d-619b4ecac17e.png)


3) Make sure you have the interfaces and the check your dmesg, you should not see the error here. 
4) Configure your switch (split-cable or not ..).
5) Plugin the ports again, if the error appears again, then there is a problem with the connectivity or the cables. 
- Make sure that you see the led blinking with yellow (Orange means there is a problem with the connection/the way it is connected) 
- If this doesn't help, replace the cables. 
6) Make sure the yellow appears for both cables (ports) in each BF and the switch also blinks with yellow. 


# References 
https://drive.google.com/open?id=1IHpo1s06yhV-4PouQiFSYelbkUWEgCpa
https://docs.mellanox.com/display/BlueFieldSWv25011176/Installing+Popular+Linux+Distributions+on+BlueField

