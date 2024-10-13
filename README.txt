Open Source Software Code Test Procedure

1. Firmware: v1.00

2. Contents:
	Copy the file "DIR853A2_V1.00B03.tar.gz" to your system and put it in "/tmp".
	Untar Open Source Software "DIR853A2_V1.00B03.tar.gz", you will get the following files.
	use command:
	# cd /tmp
	# tar -zxvf DIR853A2_V1.00B03.tar.gz

    a) DIR_853_A2_GPL_Release:The source tree directory.
    b) toolschain: The toolchains used to build the firmware.
    c) README: Instructions of building firmware.
    d) SoftwareLicenseDeclaration.pdf: Open Source Software Licenses list.
    e) LICENSE.txt: Open Source Software Licenses file.
    f) dependence:Important files for compiling successfully.	

3. How to install the system?
    a) The compile environment we used is Ubuntu Kylin 14.04.2 with i386 cpu in VMware,you can download from http://www.ubuntu.org.cn/download/ubuntu-kylin-zh-CN.
    b) Make sure the system time of the compilation OS is same with real time.

4. How to make?
    a)When the system installation is complete we need to install the compiler tool chain and make Open Source Software file, you can follow the steps below. 
        1) Login system with root user.
	   Above all,you should prepare some dependent enviroment for you system.
	    # apt-get install automake
	    # apt-get install libncurses5-dev
	    # apt-get install libstdc++6-4.4-dev
	    # apt-get install g++
	    # apt-get install bison
	    # apt-get install flex
	    # apt-get install zlib1g-dev
	    # cp /usr/bin/perl /usr/local/bin/perl
        # sudo dpkg-reconfigure dash ,select 'no'
        # sudo apt-get install doxygen		

        2) Make sure the folder "toolschain" is in the "/opt" folder ,"dependence" and "DIR_853_A2_GPL_Release" are in the "/tmp" folder.
	    # tar cjvf tool.tar.bz2 /tmp/toolschain/
	    # cp -rf /tmp/tool.tar.bz2 /opt/tool.tar.bz2
	    # cd /opt/;tar xjvf /opt/tool.tar.bz2
	   Then,you should: 
	    # cp /tmp/dependence/lzma /usr/bin/lzma
	    # cp /tmp/dependence/lzma /usr/local/bin/lzma
	    # cp /tmp/dependence/readelf /usr/bin/readelf
	   And you should make sure lzma is version 4.32.7:
	    # /usr/bin/lzma --version
	    # /usr/local/bin/lzma --version	 
	   You will see a folder named "aclocal" in "/tmp/dependence/aclocal",then copy it to "/usr/local/share/"
	    # mkdir /usr/local/share/aclocal
	    # cp -rf /tmp/dependence/aclocal/* /usr/local/share/aclocal/
        # cp -rf /tmp/dependence/libtool /usr/bin/libtool
        And you should make sure libtool is version 2.2.6b:	
        # libtool --version
        ltmain.sh (GNU libtool) 2.2.6b
        Written by Gordon Matzigkeit <gord@gnu.ai.mit.edu>, 1996

        Copyright (C) 2008 Free Software Foundation, Inc.
        This is free software; see the source for copying conditions.  There is NO
        warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.		
		
        3) Open a terminal 
	    # cd /opt/tmp/toolschain
	   You will see two folders , respectively "buildroot-gcc463" and "lzma-4.32.7"
	   Then copy the folder "buildroot-gcc463" to "/opt"
	    # tar cjvf build.tar.bz2 buildroot-gcc463/
 	    # cp build.tar.bz2 /opt;cd /opt
	    # tar xjvf build.tar.bz2			 	

        4) Go to directory "/tmp/".
            use command:
            # cd /tmp/

        5) Set execute permissions.
            use command:
	     # chmod 777 DIR_853_A2_GPL_Release -R

        6) Go to directory "DIR_853_A2_GPL_Release/".
            use command:
            # cd DIR_853_A2_GPL_Release/source

        7) Make Open Source Software.
            use command:
            # make DIR-853-A2
	   Then you should select "Kernel/Library/Defaults Selection",press button "Enter",Select "Customize Vender/User Settings" by the space bar,choose two "Exit" and choose "Yes",
	   then you will see main Menu,select "Proprietary Application" and press button "Enter",Inside Select "Support Img Crypt(NEW)" by the space bar,choose two "Exit" and choose "Yes".
	     # make dep
            # make
	
    b) The final encrypted image is under directory: /tmp/DIR_853_A2_GPL_Release/source/DIR_853_A2_firmware.img.

5.How to install?
    a) Upgrade image.img via router's Web UI. (please refer your user manual)

	   *NOTE*, I have to say, if you modified something and cause some errors,
	   the device may not be able to restart again.
	   
	   So, PLEASE MAKE SURE YOU REALLY KNOW WHAT YOU ARE DOING BEFORE YOU
	   UPGRADE ANY UNOFFICIAL FIRMWARE.
	   
6.Some hints
	The plateform we used on this project is mips, yeah, you might have
	known that after unpacking the cross compiler.
	It means, every change you want to make, could be verified in PC
	first. It's important, or you may need to restore the flash image
	much often.
	
7.Modules developed by us
	apps --- it is in the path : 	
	                DIR_853_A2_GPL_Release/source/vendor/
					DIR_853_A2_GPL_Release/source/user/rc/ 
					DIR_853_A2_GPL_Release/source/user/tw-prog.priv/ 
					DIR_853_A2_GPL_Release/source/user/apps/ 
					DIR_853_A2_GPL_Release/source/user/rt2880-app/   
	
