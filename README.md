# WebApp + UDP-API To control a WifiClock TimeMachine
## Contains a script to auto-install and auto-config on RaspberryPi(2/2B/3/3B) 
##### Auto install a WebApp that contains two main applications:
#####   1- WebApp to setup the clock - (Front End)
#####   2- WebApp (on Ulfius) to send UDP packages to the clock.

**Full install:**
1. Clone the repo:
```
git clone https://github.com/matiri132/timemachine-ctl
```
2. Give execution permissions:
```
sudo chmod +x installapi.sh
```
3. Update / upgrade
```
sudo apt update
sudo apt upgrade
```
4. Install (with SUDO):
```
sudo ./kioskMode-Rpi.sh install full
```
-Replace USER by your current user (you can create a new one for kiosk).
-If you use only install the script install and configure the app, process and services.
-if you use instal full the script will also install all necesary packages. 


5. Uninstall:
```
sudo ./installapi.sh uninstall
```
This dont uninstall all packages installed. (nginx, ulfius , yder and ocrania).
