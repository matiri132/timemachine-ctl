#include "timerstructs.h"
#include <ulfius.h>
#include <yder.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT_ULFIUS				8002
#define APPNAME				"timemach"
#define APPLOG_F			"timemach.log"
#define SERVICENAME			"clockapp"

#define COMMAND_LEN 		100
#define DATA_SIZE 			300

int EXIT_F = 1; 

/**
 * callback functions declaration
 */
int callback_post_set(const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_post_reset(const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_post_start(const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_default(const struct _u_request * request, struct _u_response * response, void * user_data);
int getclockIp(char* clk , char* address);
void sigintHandler();


/**
 * @brief Main function that handle post request from the webapp
 * 
 * @return int 
 */
int main (int argc, char **argv) {
    
    int ret;
   
    struct _u_instance instance; /**< Structura que instancia el webservice */
    
	/** Se inician los logs en el directorio de la aplicacion */
   // y_init_logs("timemach_api", Y_LOG_MODE_JOURNALD , Y_LOG_LEVEL_DEBUG, NULL, "Starting timemachine-api ");
    y_init_logs("timemach_api", Y_LOG_MODE_FILE , Y_LOG_LEVEL_DEBUG, APPLOG_F, "Starting timemachine-api ");
    printf("iniciando...\n");


    /** Se configura el puerto del webservice **/
    if (ulfius_init_instance(&instance, PORT_ULFIUS, NULL, NULL) != U_OK) {
        y_log_message(Y_LOG_LEVEL_INFO , "Error ulfius_init_instance, abort");
        return(1);
    }

    /** Declaracion de enpoints **/
    ulfius_add_endpoint_by_val(&instance, "POST", NULL, "/reset", 0, &callback_post_reset, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", NULL, "/start", 0, &callback_post_start, NULL);
	ulfius_add_endpoint_by_val(&instance, "POST", NULL, "/set", 0, &callback_post_set, NULL);
    ulfius_set_default_endpoint(&instance, &callback_default, NULL);


    /**Se inicia el framework**/
    ret = ulfius_start_framework(&instance);
   
    if (ret == U_OK) {
        y_log_message(Y_LOG_LEVEL_INFO , "Framework start OK");
        /* Se espera un SIGKILL que cambia la bandera y termina con el webservice **/
        while(EXIT_F){ }
    }else {
        y_log_message(Y_LOG_LEVEL_INFO , "Error ulfius_init_instance, abort");
    }
  	
	
	/** Se cierran los logs se frena el webservice y se limpia la memoria **/
	y_log_message(Y_LOG_LEVEL_INFO , "Closing framework");
    y_close_logs();
   	ulfius_stop_framework(&instance);
  	ulfius_clean_instance(&instance);
  
  	return 0;  

}


/**
 * @brief Callaback function for POST to /set. Get info from post and send the UDP package
 * 
 * @param request 
 * @param response 
 * @param user_data 
 * @return int 
 */

int callback_post_set(const struct _u_request * request, struct _u_response * response, void * user_data) {
    
    const char **keys; 
    const char* clock;
    char* address=o_malloc(20);
    const char* hours=o_malloc(3);
    const char* min=o_malloc(3);
    const char* sec=o_malloc(3);
    char logmesg[100] = "";

    int fsock;

    /** Validate the fields from post and if not use default values  **/
    keys = u_map_enum_keys(request->map_post_body);
   
    if(!strncmp(keys[0],"clk" , 3)){
        clock = u_map_get(request->map_post_body, keys[0]);
        if(getclockIp(clock , address) == -1 ){
            strcpy(address ,"192.168.1.225" );
        }
    }else{
        y_log_message(Y_LOG_LEVEL_INFO , "Fail to get info from webapp");
        return -1;
    }
    if(!strncmp(keys[1],"h" , 1)){
        hours = u_map_get(request->map_post_body, keys[1]);
    }else{
        y_log_message(Y_LOG_LEVEL_INFO , "Fail to get info from webapp , setting default");
        hours = "00";
    }
    if(!strncmp(keys[2],"m",1)){
        min = u_map_get(request->map_post_body, keys[2]);
    }else{
        y_log_message(Y_LOG_LEVEL_INFO , "Fail to get info from webapp , setting default");
        min = "00";
    }
    if(!strncmp(keys[3],"s" , 1)){
        sec = u_map_get(request->map_post_body, keys[3]);
    }else{
        y_log_message(Y_LOG_LEVEL_INFO , "Fail to get info from webapp , setting default");
        sec = "00";
    }        

    
    sprintf(logmesg , " -SET OK- id:%s -> %sh %sm %ss" , clock, hours, min, sec );
    y_log_message(Y_LOG_LEVEL_INFO , logmesg);

    fsock = createSocket();

    
    if(!clock_setup(fsock , address, "00" , hours, min , sec, "00", "00" )){
        ulfius_set_string_body_response(response, 200, logmesg);
    }else{
        ulfius_set_string_body_response(response, 303, "setup fail");
    }

   
  	return U_CALLBACK_CONTINUE;
}

/**
 * @brief 
 * 
 * @param request 
 * @param response 
 * @param user_data 
 * @return int 
 */
int callback_post_start(const struct _u_request * request, struct _u_response * response, void * user_data) {

    const char **keys; 
    const char* clock;
    char* address=o_malloc(20);
    char logmesg[100] = "";
    int err = 0;
    int fsock;

    /** Validate the fields from post and if not use default values  **/
    keys = u_map_enum_keys(request->map_post_body);
    
    if(!strncmp(keys[0],"clk" , 3)){
        clock = u_map_get(request->map_post_body, keys[0]);
        if(getclockIp(clock , address) == -1 ){
            strcpy(address ,"192.168.1.225" );
        }
    }else{
        y_log_message(Y_LOG_LEVEL_INFO , "Fail to get info from webapp");
        return -1;
    }
    
    fsock = createSocket();

    err = clock_start_pause(fsock , address, "01");
    if(!err ){
        sprintf(logmesg , " -START- OK id:%s " , clock);
        y_log_message(Y_LOG_LEVEL_INFO , logmesg);
	    ulfius_set_string_body_response(response, 200, logmesg);
 	}else{
        sprintf(logmesg , " -START- OK FAIL:%s error: %d " , clock , err);
        y_log_message(Y_LOG_LEVEL_INFO , logmesg);
        ulfius_set_string_body_response(response, 303, logmesg);
   }

   
  	return U_CALLBACK_CONTINUE;
}


/**
 * @brief 
 * 
 * @param request 
 * @param response 
 * @param user_data 
 * @return int 
 */
int callback_post_reset(const struct _u_request * request, struct _u_response * response, void * user_data) {
	
        
    const char **keys; 
    const char* clock=o_malloc(5);
    char* address=o_malloc(20);
    char logmesg[100] = "";

    int fsock;

    /** Validate the fields from post and if not use default values  **/
    keys = u_map_enum_keys(request->map_post_body);
   
    if(!strncmp(keys[0],"clk" , 3)){
        clock = u_map_get(request->map_post_body, keys[0]);
        getclockIp(clock , address );
    }else{
        y_log_message(Y_LOG_LEVEL_INFO , "Fail to get info from webapp");
        return -1;
    }
    
    sprintf(logmesg , " -RESET OK- id:%s " , clock );
    y_log_message(Y_LOG_LEVEL_INFO , logmesg);

    fsock = createSocket();

    
    if(!clock_setup(fsock , address, "01" , "00", "00" , "00", "00", "00" )){
        ulfius_set_string_body_response(response, 200, logmesg);
    }else{
        ulfius_set_string_body_response(response, 303, "Reset failed...");
    }

  	return U_CALLBACK_CONTINUE;
}

/**
 * @brief Funcion de callback pra responder a solcitudes por defecto.
 * 			Genera la respuesta 404.
 * @param request estructura para el request HTTP
 * @param response estructura para la respuesta HTTP 
 * @param user_data 
 * @return int 
 */
int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data) {
	ulfius_set_string_body_response(response, 404, "(USERS)Page not found, do what you want");
  	return U_CALLBACK_CONTINUE;
}

/**
 * @brief Manejador de signal para el cierre del proceso
 * Cambia el valor de la variable global EXIT_F.
 */
void sigintHandler(){ 

	signal(SIGINT, sigintHandler); 
  	EXIT_F = 0;	
	
} 

/**
 * @brief Get clock ip from /home/timemachine/.iplist
 * 
 * @param clk 
 * @param address 
 * @return int 
 */
int getclockIp(char* clk, char* address){
    FILE* file;
    char str[100];
    char *token="";
    char ip[16]="";
    const char* s = "$";
    file = fopen("/home/timemachine/.iplist" , "r");
    if(file == NULL){
        y_log_message(Y_LOG_LEVEL_INFO , "Error with the IP file");
        return -1;
    }
    while (fscanf(file, "%s", str)!=EOF){
        if(!strncmp(str , clk , 4)){
            token = strtok(str, s);
            token = strtok(NULL, s);
            strcpy(ip , token);
        }
    }
    fclose(file);
    strcpy(address , ip );
    return 0;
}