# Bluefiled on Linux

In this document we exaplain how to install Linux distributions on BlueField.
In our machine we have the host CPU which (in my example plx00) and the Arm of BF. We need to install software in both host and BF side.
First of all, we want to install the OS (in both). Then we will install the OFED. [Mellanox OFED - is a Mellanox tested and packaged version of OFED and supports two interconnect types using the same RDMA (remote DMA) and kernel bypass APIs called OFED verbs – InfiniBand and Ethernet.]
Secondly, we will Enable the interface to make a bridge [ping] from the BF. 
Finally, you can test you BF via the ib_xxx tests of the OFED.


# Bluefiled on Ubuntu 18.04
## Requirment 
1) Host system is Ubuntu 18.04
2) BlueField NIC inserted in a host PCIe slot
3) USB cable connecting the NIC card and the host

## preinstall
go to:
https://www.mellanox.com/products/software/bluefield
install From BlueField Ubuntu Server 18.04

### OFED in the Host:
cd BlueField-2.2.0.11000/distro/rhel/pxeboot

### HWE
sudo apt install linux-signed-generic-hwe-18.04
sudo reboot

### RShim driver
sudo apt install build-essential debhelper autotools-dev dkms
cd $BSP_PATH/src/drivers/rshim/
make -C /lib/modules/`uname -r`/build M=$PWD

dpkg-buildpackage -us -uc -nc
sudo dpkg -i ../rshim-dkms_*.deb
sudo modprobe rshim_usb
sudo modprobe rshim_net

vim /etc/udev/rules.d/91-tmfifo_net.rules
SUBSYSTEM=="net", ACTION=="add", ATTR{address}=="00:1a:ca:ff:ff:02", ATTR{type}=="1", NAME="tmfifo_net0", RUN+="/usr/sbin/ifup tmfifo_net0"
vim /sbin/ifup-local  
    INTF=$1
if [ "$INTF" = "tmfifo_net0" ]; then
  systemctl restart dhcpd
     Fi
chmod +x /sbin/ifup-local
Reboot
ip add  (If you still see that interface tmfifo_net is still not up. then the OFED was not installed)
mount mlnx_ofed.iso /mnt
sudo /mnt/mlnxofedinstall --add-kernel-support --distro rhel7.4 (if it complains about uninstalling old ofed, run:]
   $./uninstall --force)

ip ad (You should see  interface tmfifo_net  here)

sudo ifconfig tmfifo_net0 192.168.100.1/24 up

outgoing network interface on the host needs to be up, there are two ports:
$sudo ifconfig *ens6f0 192.168.0.20 up
$sudo ifconfig *ens6f1 192.168.0.21 up

SmartNIC has a private network to the host via the USB connection, and it can be used to Secure Copy Protocol (SCP) all the required packages. However, it is recommended for the SmartNIC to have a direct access to the network to use “yum install” to install all the required packages. For direct access to the network, set up the routing on the host via [when machine is rebooted , run it again]:
$iptables -t nat -A POSTROUTING -o *enp5s0f0 -j MASQUERADE
$echo 1 > /proc/sys/net/ipv4/ip_forward
$ sudo systemctl restart dhcpd

*enp5s0f0  the outgoing network interface on the host.


/etc/init.d/openibd start

### Install the image to Bluefiled (Ubuntu 18.04 and OFED):
     cat <ubuntu.bfb> > /dev/rshim0/boot
Username: ssh ubuntu@192.168.100.1
Password: ubuntu



# Bluefiled on Centos 
TBD

# References 
https://drive.google.com/open?id=1IHpo1s06yhV-4PouQiFSYelbkUWEgCpa
