#!/bin/bash
##########SETABLES######
APPDIR="/home/timemachine"
DOMAINNAME='localhost'
#########################

WD=$(pwd)

install_packages (){
	#Install dependencies
	echo "INSTALLING DEPENDENCIES..."
	#Nginx dep
	if [ ! -x "$(command -v nginx))" ]; then
		echo "Installing NGINX..."
		apt install nginx -y
	fi
	apt install -y libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev libgnutls28-dev libgcrypt20-dev libsystemd-dev
	mkdir soft
    cd ${WD}soft
	if [ ! -f "/usr/local/lib/liborcania.so" ]
	then	
		git clone https://github.com/babelouest/orcania.git
		cd ${WD}/orcania/
    	make
    	make install
	fi
	if [ ! -f "/usr/local/lib/libyder.so" ]
	then
		git clone https://github.com/babelouest/yder.git
    	cd ${WD}/yder/
    	make
    	make install
	fi
	if [ ! -f "/usr/local/lib/libulfius.so" ]
	then    
        git clone https://github.com/babelouest/ulfius.git
	    cd ${WD}/ulfius/
	    make
	    make install
	fi
	
    	
   
}

case $1 in
	install)
		
		if [ $2 == 'full' ]
		then
			install_packages
		fi
		echo "INSTALLING WEBAPP..."								
		#Create user to serve webservice
		useradd -m -g www-data -s /bin/bash timemachine
		
		#Configure ulfius api
		cp ${WD}/config/iplist ${APPDIR}/.iplist
        cd ${WD}/udpapi 
             
        make
        cp ${WD}/udpapi/timemachineapi /usr/local/sbin/
		make clean
  		
		#Create service
		cp ${WD}/config/clockapp.service /etc/systemd/system/clockapp.service

		systemctl enable clockapp
		systemctl start clockapp		
		
		#Configure NGINX
		echo "CONFIGURING NGINX..."
		cp -r ${WD}/webapp /var/www/html/webclock
		
		rm /etc/nginx/sites-enabled/default
		cp ${WD}/config/clocksite /etc/nginx/sites-available/clocksite
		ln -s /etc/nginx/sites-available/clocksite /etc/nginx/sites-enabled/default

		if [ -x "$(command -v ufw))" ]
		then
			echo "Configure firewall..."
			ufw allow 'Nginx Full'
		fi

		systemctl restart nginx

		echo "INSTALL COMPLETE"
	;;

	uninstall)
		#uninstall api
		rm -Rf /var/www/html/webclock
		systemctl stop clockapp
		systemctl disable clockapp
		rm /etc/systemd/system/clockapp.service
		rm /usr/local/sbin/timemachine
		userdel -r timemachine
		#Set default nginx
		rm -f /etc/nginx/sites-enabled/default
		rm -f /etc/nginx/sites-available/clocksite
		ln -s /etc/nginx/sites-available/default /etc/nginx/sites-enabled/
		echo "Reloading nginx..."
		systemctl reload nginx
		systemctl daemon-reload
		echo "Done."
	;;
esac
if [ $1 == 'help' ]
then 
	echo "help"
fi

