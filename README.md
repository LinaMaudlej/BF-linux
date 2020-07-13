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
* Download (to host machine) MLNX_OFED. <mlnx_ofed>
* Download BlueField BlueOS. <BlueOS_dir>
* Download (to host machine) BlueField Ubuntu Server 18.04 image. <bf_img.bfb>

### HWE
	sudo apt install linux-signed-generic-hwe-18.04
	sudo reboot --force
	sudo apt install build-essential debhelper autotools-dev dkms

### In the Host:

#### OFED: 
	mount <mlnx_ofed>.iso /mnt
	sudo /mnt/uninstall --force
	sudo /mnt/mlnxofedinstall --add-kernel-support 
You should see that it is installed: 
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
 

#### rshim drivers: 
	cd <BlueOS_dir>/src/drivers/rshim
	sudo modprobe -vr rshim_pcie
	sudo modprobe -vr rshim_net
	sudo modprobe -vr rshim_usb

##### First installation option: 

	dpkg-buildpackage -us -uc -nc
	sudo dpkg -i ../rshim-dkms_*.deb
	sudo modprobe rshim_usb
	sudo modprobe rshim_net
	
##### If the "First" did not work, pease do install it manually by:

	make -C /lib/modules/`uname -r`/build M=$PWD
	sudo modprobe rshim_usb
	sudo modprobe rshim_net
	
	vim /etc/udev/rules.d/91-tmfifo_net.rules
Add this line to tmfifo_net.rules
SUBSYSTEM=="net", ACTION=="add", ATTR{address}=="00:1a:ca:ff:ff:02", ATTR{type}=="1", NAME="tmfifo_net0", RUN+="/usr/sbin/ifup tmfifo_net0"
	
	reboot --force
	ip add
	
You should be able to see interface tmfifo_net and two outgoing network interfacs.
	
	sudo ifconfig tmfifo_net0 192.168.100.1/24 up
	sudo ifconfig <first_interface> 192.168.0.20 up
	sudo ifconfig <second_interface> 192.168.0.21 up
--photo



### In the BF:

### Install the image to Bluefiled (Ubuntu 18.04 and OFED):
     cat  <bf_img.bfb> > /dev/rshim0/boot
Username: ssh ubuntu@192.168.100.2
Password: ubuntu





# Bluefiled on Centos 
TBD

# References 
https://drive.google.com/open?id=1IHpo1s06yhV-4PouQiFSYelbkUWEgCpa
https://docs.mellanox.com/display/BlueFieldSWv25011176/Installing+Popular+Linux+Distributions+on+BlueField

