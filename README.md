# linkit-smart-feed
This feeds holds the config/meta package for the LinkIt Smart 7688 (Duo) for OpenWrt 18.06.

It uses the [mt76](https://github.com/openwrt/mt76) driver for wifi.

# Build the firmware from sources

This section describes how to build the firmware for LinkIt Smart 7688 and LinkIt Smart 7688 Duo from source codes.


### Host environment
The following operations are performed under a Debian Stretch 9.5 environment. For a Windows or a Mac OS X host computer, you can install a VM for having the same environment:
* Download Debian Stretch 9.5 image from [http://www.debian.org](http://www.debian.org)
* Install this image with VirtualBox (http://virtualbox.org) on the host machine. 50GB disk space reserved for the VM is recommanded


### Steps
In the Debian system, open the *Terminal* application and type the following commands:

1. Install prerequisite packages for building the firmware:
    ```
    $ sudo apt-get install git g++ make libncurses5-dev subversion libssl-dev gawk libxml-parser-perl unzip wget python xz-utils
    ```

2. Download OpenWrt CC source codes:
    ```
    $ git clone https://git.openwrt.org/openwrt/openwrt.git/ -b openwrt-18.06
    ```
    
3. Prepare the default configuration file for feeds:
    ```
    $ cd openwrt
    $ cp feeds.conf.default feeds.conf
    ```
    
4. Add the LinkIt Smart 7688 feed:
    
    ```
    $ echo src-git linkit https://github.com/Kenzu/linkit-smart-7688-feed.git >> feeds.conf
    ```
5. Update the feed information of all available packages for building the firmware:
    
    ```
    $ ./scripts/feeds update -a
    ```
6. Install all packages:
    
    ```
    $ ./scripts/feeds install -a
    ```
7. Prepare the kernel configuration to inform OpenWrt that we want to build an firmware for LinkIt Smart 7688:
    
    ```
    $ make menuconfig
    ```
    * Select the options as below:
        * Target System: `MediaTek Ralink MIPS`
        * Subtarget: `MT76x8 based boards`
        * Target Profile: `MediaTek LinkIt Smart 7688`
    * Save and exit (**use the deafult config file name without changing it**)
    * build kmod except these,
    ```
    $ grep -i kmod .config | grep -v m$ | grep -v y$
    ```
8. Start the compilation process:
    
    ```
    $ make V=99
    ```
9. After the build process completes, the resulted firmware file will be under `bin/targets/ramips/mt76x8/openwrt-ramips-mt76x8-LinkIt7688-squashfs-sysupgrade.bin`. Depending on the H/W resources of the host environment, the build process may **take more than 2 hours**.

10. You can use this file to do the firmware upgrade through the Web UI. Or rename it to `lks7688.img` for upgrading through a USB drive.



#Get Started with Linkit Smart 7688


##Firmware and bootloader 

- ####connect to the system console of LinkIt Smart 7688 development boards by using a Serial (or UART) to USB converter.

###
>	$ ls /dev/ttyUSB*
>	/dev/ttyUSB0


###
>	picocom -ez -b57600 /dev/ttyUSB0

- ####Modifying the bootloader console

    1.  Get bootloader source code
      
        Clone the **UBOOT** source from [GitHub](https://github.com/MediaTek-Labs/linkit-smart-7688-uboot) GitHub repository. 
      
    2. Modify the console configuration
        Modify the file "board/rt2880/serial.h" by defining a different port for **CFG_RT2880_CONSOLE**.
>  #define CFG_RT2880_CONSOLE    RT2880_UART1
>  //#define CFG_RT2880_CONSOLE    RT2880_UART3 
        
    3. To change the baud rate, modify the file "include/configs/rt2880.h":
        
        change the bootloader console baud rate from **57600** to **115200**.
>  #define CONFIG_BAUDRATE 115200


    4. Build and upload the modified bootloader
    
        Now [build](https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/tutorials/firmware-and-bootloader/build-the-bootloader) build the bootloader and [upload](https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/tutorials/firmware-and-bootloader/updating-the-bootloader) upload it to the board.


- ####Modifying the kernel console
To use a different UART port for the kernel console, you can follow the steps below to adjust the configuration.

    1.  Get firmware source code
Follow the [descriptions](./BUILD.md) descriptions descriptions to get the source code of the firmware.

    2. Modify the console configuration
Modify the file " target/linux/ramips/dts/LINKIT7688.dts", and replace this

>  chosen {
>      bootargs = "console=ttyS0,115200";
>  }; 

   Now build the firmware and [upload](https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/get-started/get-started-with-the-linkit-smart-7688-development-board/update-your-boards-firmware)  it to the board through the  [Web UI](https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/tutorials/firmware-and-bootloader/update-the-firmware-with-web-ui)   or the  [USB drive](https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/tutorials/firmware-and-bootloader/update-the-firmware-with-a-usb-drive)  .



##Wi-Fi LED states

![LinkIt Smart 7688 development board doesn't have a display that shows information such as system states through a GUI. Instead, it utilizes the on-board Wi-Fi LED to provide information about the system states. Understanding the LED patterns and corresponding system states can help you to identify and diagnose problems.The on-board Wi-Fi LED status indicator is located near the PWR micro-USB port and labeled as WiFi. This document introduces the LED behavior and its meaning under different circumstances. The image below gives a summary of the LED states.](./LED_states.png) 

####In AP mode
There are 2 Wi-Fi LED states in AP mode:

- The LED is off. It means no client device is connected to the LinkIt Smart 7688 development board.
- The LED blinks 3 times per second and pauses for 0.5 seconds, and repeats the pattern. This indicates there is at least 1 client device connected to the LinkIt Smart 7688 development board.
####In Station mode
There are 3 Wi-Fi LED states in Station mode:

- The LED is off. The LinkIt Smart 7688 development board failed to connect to a wireless router and is timed-out.
- The LED blinks twice per second continuously. It indicates the LinkIt Smart 7688 development board is connecting to a wireless router.
- The LED blinks according to data transmission. The LinkIt Smart 7688 development board has connected to a wireless router and the Wi-Fi LED will blink as data is transmitting.

####Copying files to the board

	scp ./helloworld root@mylinkit.local:/example/helloworld



#Network

####Switch to Station mode

	# uci set wireless.sta.ssid=SampleAP
	# uci set wireless.sta.key=12345678
	# uci set wireless.sta.encryption=psk2
	# uci commit
	# wifi_mode sta

####Switch to AP mode

	# wifi_mode ap
	# wifi

####Switch to Repeater mode

- Step 1 — Use UCI commands to assign repeater SSID, password and information of the network to be connected.

> 
	# uci set wireless.sta.ssid=SampleAP
	# uci set wireless.sta.key=12345678
	# uci set wireless.sta.encryption=psk2
	# uci set wireless.ap.ssid=MyRepeater
	# uci commit

- Step 2 — Type a command to restart the Wi-Fi driver to activate the Repeater configuration.

> 
	# wifi_mode apsta
	
####Setup a wireless router
