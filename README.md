# Ubertooth-Win

A professional > [Ubertooth-One](https://greatscottgadgets.com/ubertoothone/) tool for researching and debugging BT and BLE bluetooth protocols. 
This package is made to work on Windows OS. Read the description of the package for Linux on the page: https://github.com/greatscottgadgets/ubertooth

![ubertooth-and-antenna](https://github.com/sevstels/Ubertooth-Win/assets/13213368/2c892a59-b8d8-4965-8203-b17a37e11778)


Release of the Windows version of the device management GUI utility.   
To connect the device, install the WinUSB driver with Zadig utility.   
https://zadig.akeo.ie/  

Open the utility and select the: <b>Options->List All Devices</b>.  
Find the <b>bluetooth_rxtx</b> device in the list and install the latest WinUSB driver for it.  

Add the necessary Microsoft dll`s to the operating systems.  

vcredist_x64.exe - for 64-bit versions of Windows 11, Windows 10 and previous OS.  
https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x64.exe/  

vcredist_x86.exe - for 32-bit systems.    
https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe/  

<p align="center">A few functions I need are working now.<p/>
<p align="center"><img src="https://github.com/sevstels/Ubertooth-Win/assets/13213368/e50a8506-a3fa-41db-8ea2-74f98c510b88"><p/>
<p align="center">Spectrum analyzer:<p/>

<p align="center"><img src="https://github.com/sevstels/Ubertooth-Win/assets/13213368/10f5888d-cb91-4ae8-8055-34f7c11a1ed8"><p/>

<p align="center">Read data stream and convenient channel switching<p/>

<p align="center"><img src="https://github.com/sevstels/Ubertooth-Win/assets/13213368/86b4bd34-3af3-47d0-884b-5483a8d8ea1b"><p/>
  
A little later I will add forward received packets to WireShark. You can download the release at this link: [Ubertooth-Win Release](https://gradient-sg.com/files/ubert/UW-Release.zip) 
Because of Microsoft's new ruls 2FA, makes do very difficult to log into GitHub if you have releases here.
