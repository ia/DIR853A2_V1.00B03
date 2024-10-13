<onetouch> directory

A. Libraries: 
   1. There are the library files *.so that should be copied into your system 
      lib directory.
   2. There are some c/obj/include files of libstarter.so.
   3. Please compile the libstarter.so by yourself after implement the 
      functions in platform.c file.
   4. Please compile the easyroaming by yourself after implement functions in
      "onetouch/easyroaming/platform.c".
	     - int GetStaInfo()
		 - int Platform_Get_Connected_STA()
		 - int Platform_Deauth_STA()
		 - int Platform_Get_Wireless_Cofnig()

B. Daemons:
   There are the daemons used by One Touch.
   1. onetouch -- provide Onetouch-App/AP_Clonig/Easy_Roaming Services
   2. goahead -- provide RSS access features used by One Touch
   3. dxml -- provide DXml DB access feature and Interpreter
   4. easyroaming -- provide EasyRoaming Ver. 2 service. (a standalone daemon
      used to replace with onetouch build-in old version)

C. Scripts
   In the 'script' directory, there are the scripts files used by One Touch. 

D. Web page files:
   In the 'www' directory, there are the scripts files used by One Touch. 
   1. In the 'rssui' directory, there are the used HTML files used for RssUI 
      feature. Please put these files into goahead's www directory.
   2. In the 'rsstest' directory, there are the test HTML pages used for debug
      RssSet and FwUpgrade functions. Please put these files into goahead's www 
      directory when you want to debug these features.

<source> directory

   In the 'goahead' directory, there are the its source codes. Please use 
   keyword 'D-Link' to search our modifications. You can remove our goahead 
   daemon after porting these modified codes into your http daemon.
   
