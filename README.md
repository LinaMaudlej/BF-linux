# Bluefiled(BF) on Linux

In this document we exaplain how to install Linux distributions on BlueField.
In our machine we have the host CPU and the Arm of BF. We need to install software in both host and BF side.
Firstly, we will install the OFED in the host. [Mellanox OFED - is a Mellanox tested and packaged version of OFED and supports two interconnect types using the same RDMA (remote DMA) and kernel bypass APIs called OFED verbs – InfiniBand and Ethernet.]
Secondly, we will install OFED + Ubuntu in the BF.
Thirdly, we will set the configuration of the BF (Sperated mode vs Embedded mode)
Finally, we will our OFED with one example with RDMA and another with OFED examples.


# Bluefiled on Ubuntu 18.04
## Requirment 
1) Host system is Ubuntu 18.04
2) BlueField NIC inserted in a host PCIe slot
3) USB cable connecting the NIC card and the host

## preinstall
Go to:
https://www.mellanox.com/products/software/bluefield
*Install BlueField Ubuntu Server 18.04
*Install 

### OFED in the Host:
cd BlueField-2.2.0.11000/distro/rhel/pxeboot
See log file /tmp/MLNX_OFED_LINUX-4.7-3.2.9.0-5.3.0-62-generic/mlnx_iso.8061_logs/mlnx_ofed_iso.8061.log

Checking if all needed packages are installed...
Building MLNX_OFED_LINUX DEBS . Please wait...
sudo apt install docker.io kubernetes-client
Creating metadata-rpms for 5.3.0-62-generic ...
WARNING: If you are going to configure this package as a repository, then please note
WARNING: that it is not signed, therefore, you need to set 'trusted=yes' in the sources.list file.
WARNING: Example: deb [trusted=yes] file:/<path to MLNX_OFED DEBS folder> ./
Created /tmp/MLNX_OFED_LINUX-4.7-3.2.9.0-5.3.0-62-generic/MLNX_OFED_LINUX-4.7-3.2.9.0-ubuntu18.04-ext.tgz
Removing old packages...
Installing /tmp/MLNX_OFED_LINUX-4.7-3.2.9.0-5.3.0-62-generic/MLNX_OFED_LINUX-4.7-3.2.9.0-ubuntu18.04-ext
/tmp/MLNX_OFED_LINUX-4.7-3.2.9.0-5.3.0-62-generic/MLNX_OFED_LINUX-4.7-3.2.9.0-ubuntu18.04-ext/mlnxofedinstall --force --without-dkms 
Logs dir: /tmp/MLNX_OFED_LINUX.6334.logs
General log file: /tmp/MLNX_OFED_LINUX.6334.logs/general.log

Below is the list of MLNX_OFED_LINUX packages that you have chosen
(some may have been added by the installer due to package dependencies):

ofed-scripts
mlnx-ofed-kernel-utils
mlnx-ofed-kernel-modules
rshim-modules
iser-modules
srp-modules
libibverbs1
ibverbs-utils
libibverbs-dev
libibverbs1-dbg
libmlx4-1
libmlx4-dev
libmlx4-1-dbg
libmlx5-1
libmlx5-dev
libmlx5-1-dbg
librxe-1
librxe-dev
librxe-1-dbg
libibumad
libibumad-static
libibumad-devel
ibacm
ibacm-dev
librdmacm1
librdmacm-utils
librdmacm-dev
mstflint
ibdump
libibmad
libibmad-static
libibmad-devel
libopensm
opensm
opensm-doc
libopensm-devel
infiniband-diags
infiniband-diags-compat
mft
kernel-mft-modules
libibcm1
libibcm-dev
perftest
ibutils2
libibdm1
cc-mgr
ar-mgr
dump-pr
ibsim
ibsim-doc
mxm
ucx
sharp
hcoll
openmpi
mpitests
knem-modules
libdapl2
dapl2-utils
libdapl-dev
srptools
mlnx-ethtool
mlnx-iproute2

This program will install the MLNX_OFED_LINUX package on your machine.
Note that all other Mellanox, OEM, OFED, RDMA or Distribution IB packages will be removed.
Those packages are removed due to conflicts with MLNX_OFED_LINUX, do not reinstall them.

Checking SW Requirements...
Removing old packages...
Installing new packages
Installing ofed-scripts-4.7...
Installing mlnx-ofed-kernel-utils-4.7...
Installing mlnx-ofed-kernel-modules-4.7...
Installing rshim-modules-1.16...
Installing iser-modules-4.7...
Installing srp-modules-4.7...
Installing libibverbs1-41mlnx1...
Installing ibverbs-utils-41mlnx1...
Installing libibverbs-dev-41mlnx1...
Installing libibverbs1-dbg-41mlnx1...
Installing libmlx4-1-41mlnx1...
Installing libmlx4-dev-41mlnx1...
Installing libmlx4-1-dbg-41mlnx1...
Installing libmlx5-1-41mlnx1...
Installing libmlx5-dev-41mlnx1...
Installing libmlx5-1-dbg-41mlnx1...
Installing librxe-1-41mlnx1...
Installing librxe-dev-41mlnx1...
Installing librxe-1-dbg-41mlnx1...
Installing libibumad-43.1.1.MLNX20190905.1080879...
Installing libibumad-static-43.1.1.MLNX20190905.1080879...
Installing libibumad-devel-43.1.1.MLNX20190905.1080879...
Installing ibacm-41mlnx1...
Installing ibacm-dev-41mlnx1...
Installing librdmacm1-41mlnx1...
Installing librdmacm-utils-41mlnx1...
Installing librdmacm-dev-41mlnx1...
Installing mstflint-4.13.0...
Installing ibdump-5.0.0...
Installing libibmad-5.4.0.MLNX20190423.1d917ae...
Installing libibmad-static-5.4.0.MLNX20190423.1d917ae...
Installing libibmad-devel-5.4.0.MLNX20190423.1d917ae...
Installing libopensm-5.5.1.MLNX20191120.0c8dde0...
Installing opensm-5.5.1.MLNX20191120.0c8dde0...
Installing opensm-doc-5.5.1.MLNX20191120.0c8dde0...
Installing libopensm-devel-5.5.1.MLNX20191120.0c8dde0...
Installing infiniband-diags-5.4.0.MLNX20190908.5f40e4f...
Installing infiniband-diags-compat-5.4.0.MLNX20190908.5f40e4f...
Installing mft-4.13.3...
Installing kernel-mft-modules-4.13.3...
Installing libibcm1-41mlnx1...
Installing libibcm-dev-41mlnx1...
Installing perftest-4.4...
Installing ibutils2-2.1.1...
Installing libibdm1-1.5.7.1...
Installing cc-mgr-1.0...
Installing ar-mgr-1.0...
Installing dump-pr-1.0...
Installing ibsim-0.7mlnx1...
Installing ibsim-doc-0.7mlnx1...
Installing mxm-3.7.3112...
Installing ucx-1.7.0...
Installing sharp-2.0.0.MLNX20190922.a9ebf22...
Installing hcoll-4.4.2938...
Installing openmpi-4.0.2rc3...
Installing mpitests-3.2.20...
Installing knem-modules-1.1.3.90mlnx1...
Installing libdapl2-2.1.10mlnx...
Installing dapl2-utils-2.1.10mlnx...
Installing libdapl-dev-2.1.10mlnx...
Installing srptools-41mlnx1...
Installing mlnx-ethtool-5.1...
Installing mlnx-iproute2-5.2.0...
Selecting previously unselected package mlnx-fw-updater.
(Reading database ... 248147 files and directories currently installed.)
Preparing to unpack .../mlnx-fw-updater_4.7-3.2.9.0_amd64.deb ...
Unpacking mlnx-fw-updater (4.7-3.2.9.0) ...
Setting up mlnx-fw-updater (4.7-3.2.9.0) ...

Added 'RUN_FW_UPDATER_ONBOOT=no to /etc/infiniband/openib.conf

Attempting to perform Firmware update...
Querying Mellanox devices firmware ...

Device #1:
----------

  Device Type:      BlueField
  Part Number:      MBF1M332A-ASCA_Ax
  Description:      BlueField(TM) SmartNIC 25GbE dual-port SFP28; PCIe Gen3.0/4.0 x8; BlueField(TM) G-Series 16 Cores; Crypto enabled; 16GB on-board DDR; HHHL; ROHS R6
  PSID:             MT_0000000230
  PCI Device Name:  02:00.0
  Base GUID:        98039b0300cc223c
  Base MAC:         98039bcc223c
  Versions:         Current        Available     
     FW             18.26.4012     18.26.4012    
     NVMe           20.1.0003      20.1.0003     
     PXE            3.5.0805       3.5.0805      
     UEFI           14.19.0017     14.19.0017    

  Status:           Up to date


Log File: /tmp/MLNX_OFED_LINUX.6334.logs/fw_update.log
Device (02:00.0):
	02:00.0 Ethernet controller: Mellanox Technologies MT416842 BlueField integrated ConnectX-5 network controller
	Link Width: x8
	PCI Link Speed: 8GT/s

Device (02:00.1):
	02:00.1 Ethernet controller: Mellanox Technologies MT416842 BlueField integrated ConnectX-5 network controller
	Link Width: x8
	PCI Link Speed: 8GT/s

Device (02:00.2):
	02:00.2 DMA controller: Mellanox Technologies MT416842 BlueField SoC management interfac
	Link Width: x8
	PCI Link Speed: 8GT/s

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
