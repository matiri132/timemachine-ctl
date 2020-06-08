#include "timerstructs.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>



/**
 * @brief Converts a 2 digit number in string format to a byte
 * 
 * @param digits 2 digit string
 * @return uint8_t 
 */
uint8_t strToByte(char* digits){
    
    if(strlen(digits)!=2 ){
        return 0;
    }else{
        if(((uint8_t)digits[0]<48 || (uint8_t)digits[0]>57)||
            ((uint8_t)digits[1]<48 || (uint8_t)digits[1]>57)){
                return 0;
        }else{
            return ((uint8_t)(digits[0] - 48)*10 + (uint8_t)(digits[1] - 48));
        }
    }
}

/**
 * @brief Create a Socket to the address passed as argument and PORT.
 * 
 * @param address IP destination address
 * @return int 
 */
int createSocket(){
    
    int sockfd; 
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        return -1;
    }     
    return sockfd; 
} 

/**
 * @brief Sends the info msg to the clock and receive the info in a 
 * tmach_response structure.
 * 
 * @param sockfd file descriptor of socket 
 * @param servaddr sockadrr_in with the ip of the  clock
 * @return tmach_response 
 */
tmach_response clock_info(int sockfd , char* address){

    int err , n;
    tmach_response response = {0};
    clock_ctl info_msg = {0xA1 , 0x04 , 0xB2};

    /** Create sock address struct **/
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    int addassign = inet_aton(address , &servaddr.sin_addr);
	if(addassign == 0){
		perror("Addres not valid");
		return response;
	}
    
    err = sendto(sockfd, &info_msg , 3, 0, 
                (const struct sockaddr *) &servaddr, (socklen_t)sizeof(servaddr)); 

    if(err == 0){
        perror("Info package not sended");
       
    }else if(err == -1){
        perror("UDP error ");
    }else{
        n = recvfrom(sockfd, &response, 35, MSG_WAITALL,
                    (struct sockaddr *) &servaddr, (unsigned int* restrict)sizeof(servaddr)); 
        if(n == 35){
            return response;
        }else{
            perror("Corrupted info.");
            return response;
        }

    }
    return response;
}

/**
 * @brief Sends a setup message to the clock, to use down timer or reset
 * 
 * @param sockfd Socket to send the package.
 * @param address IP address
 * @param reset 00 to init , 01 to reset timer.
 * @param hours Hours to set (in 2 digit string).
 * @param mins Minutes to set (in 2 digit string).
 * @param sec seconds to set (in 2 digit string).
 * @param alarm 1 to enable alarm and 0 to disable.
 * @param al_dur duration of the alarm in seconds.
 * @return int 
 */
int clock_setup(int sockfd , char* address, char* reset,
                char* hours, char* mins, char* sec, char* alarm, char* al_dur){
    int err;
    int n;
    uint8_t response;
    clock_d_ctl setup_msg;
    
    /** Create sock address struct **/
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    int addassign = inet_aton(address , &servaddr.sin_addr);
	if(addassign == 0){
		perror("Addres not valid");
		return -1;
	}
    
    if(strToByte(reset) == 0){
        setup_msg.command = (uint8_t)0xA5; /**< Use down timer **/
    }else{
        setup_msg.command = (uint8_t)0xA7; /**< Reset down timer **/
    }    
    setup_msg.displaymode = (uint8_t)0x01;
    setup_msg.displaytime.hour = strToByte(hours);
    setup_msg.displaytime.minutes = strToByte(mins);
    setup_msg.displaytime.seconds = strToByte(sec);
    setup_msg.start_t_sec = (uint8_t)0x00;
    setup_msg.alarm = strToByte(alarm);
    setup_msg.alarm_dur = strToByte(al_dur);

    err = sendto(sockfd, &setup_msg , 8, 0, 
                (const struct sockaddr *) &servaddr, (socklen_t)sizeof(servaddr)); 

    if(err == 0){
        perror("Setup package not sended");
        return -1;
    }else if(err == -1){
        perror("UDP error ");
        return -1;
    }else{
        n = recvfrom(sockfd, &response, 1, MSG_WAITALL,
                    (struct sockaddr *) &servaddr, (unsigned int* restrict)sizeof(servaddr)); 
        if((response == (uint8_t)'A')){
            return 0;
        }else{
            perror("Corrupted info");
            return -1;
        }

    }

}


/**
 * @brief Send message to start or pause the timer
 * 
 * @param sockfd Socket to send the package
 * @param servaddr structu with the address and port to send the address
 * @param mode 00 to pause, 01 to start.
 * @return int 
 */
int clock_start_pause(int sockfd , char* address , char* mode ){
    
    int err;
    int n;
    uint8_t response;
    clock_use use_msg;
    /** Create sock address struct **/
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    int addassign = inet_aton(address , &servaddr.sin_addr);
	if(addassign == 0){
		perror("Addres not valid");
		return -1;
	}
    /** Create the UDP content **/
    use_msg.command = 0xA6;
    use_msg.none = 0x00;
    if(strToByte(mode)== 1){
        use_msg.run = 0x01;
    }else{
        use_msg.run = 0x00;
    }

    err = sendto(sockfd, &use_msg , 3, 0, 
                (const struct sockaddr *) &servaddr, (socklen_t)sizeof(servaddr)); 

    if(err == 0){
        perror("Setup package not sended");
        return -3;
    }else if(err == -1){
        perror("UDP error ");
        return -4;
    }else{
        n = recvfrom(sockfd, &response, 1, MSG_WAITALL,
                    (struct sockaddr *) &servaddr, (unsigned int* restrict)sizeof(servaddr)); 
        if(response == (uint8_t)'A'){
            return 0;
        }else{
            perror("Corrupted info.");
            return -4;
        }

    }
}

/**
 * @brief Set up clock while running.
 * 
 * @param sockfd Socket to send the package
 * @param servaddr Struct to handle addres and port
 * @param hours Hours in 2 digit string
 * @param mins Minutes in 2 digit string
 * @param sec Sec in 2 digit string
 * @param t_sec tenths of seconds in 2 digit string
 * @param h_sec houndredths of second in 2 digit string
 * @return int 
 */
int clock_running(int sockfd , char* address,
                 char* hours, char* mins, char* sec, char* t_sec, char* h_sec){
    int n;
    int err;
    uint8_t response;
    clock_r_ctl msg_w_run;
    /** Create addres struct **/
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    int addassign = inet_aton(address , &servaddr.sin_addr);
	if(addassign == 0){
		perror("Addres not valid");
		return -1;
	}
    msg_w_run.command = 0xAB;
    msg_w_run.displaytime.hour = strToByte(hours);
    msg_w_run.displaytime.minutes = strToByte(mins);
    msg_w_run.displaytime.seconds = strToByte(sec);
    msg_w_run.new_t_sec = strToByte(t_sec);
    msg_w_run.new_h_sec = strToByte(h_sec);

    err = sendto(sockfd, &msg_w_run , 6, 0, 
                (const struct sockaddr *) &servaddr, (socklen_t)sizeof(servaddr)); 

    if(err == 0){
        perror("Setup package not sended");
        return -1;
    }else if(err == -1){
        perror("UDP error ");
        return -1;
    }else{
        n = recvfrom(sockfd, &response, 1, MSG_WAITALL,
                    (struct sockaddr *) &servaddr, (unsigned int* restrict)sizeof(servaddr)); 
        if(response == (uint8_t)'A'){
            return 0;
        }else{
            perror("Corrupted info.");
            return -1;
        }

    }
    
    
}
