/*

	Diego Garcia (diegargon) - diego@envigo.net

*/


#include "domo.h"

static int running = 0;

static char *pid_file_name = NULL;
static int pid_fd = -1;

int main(int argc, char *argv[]) {
	
	int reconnect_time = RECONNECT_TIME;
	int value, option_index = 0;
	int daemon = 0;
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"daemon", no_argument, 0, 'd'},
		{NULL, 0, 0, 0}
	};	
	//unsigned long int SubsNcount = 0;
	GKeyFile *conf = g_key_file_new ();
	
	while ((value = getopt_long(argc, argv, "p:dh", long_options, &option_index)) != -1) {
		switch (value) {
			case 'p':
				pid_file_name = strdup(optarg);
				break;			
			case 'd':
				daemon = 1;
				break;
			case 'h':
				print_help(argv[0]);
				return EXIT_SUCCESS;
			case '?':
				print_help(argv[0]);
				return EXIT_FAILURE;
			default:
				break;
		}
	}	
	
	if(daemon) {
		daemonize();
	}

	/* Signal */
	signal(SIGUSR1, sig_handler);
	signal(SIGINT, sig_handler);
	/**/

	domoCfg_Init(conf);
	log_init(APP_NAME, conf, daemon);	
	
	//pinConfig *pinConf = g_malloc0(sizeof(struct pinConfig));
	int NumPins = domCfg_getNumElementsList( conf, "WIRING", "PinConf" );	
	pinConfig pinConf[NumPins];
		
		
	WiringInit(conf, pinConf, NumPins);
	printPinConfig(pinConf, NumPins);
	
	//SubsNcount = domCfg_getNumElementsList( conf, "MQTT", "SubsTopics" );		
	//mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context) +  (sizeof(char *) * SubsNcount));
	mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context) + (sizeof(struct pinConfig) * NumPins) );
	ctx->NumPins = NumPins;
	ctx->pinConf = pinConf;

	
	MQTT_Init(ctx, conf);	
	MQTT_connect(ctx);
	
	running = 1;

	while (!MQTTAsync_isConnected(ctx->client) && running ) 
	{
		log_msg(LOG_DEBUG,"MQTT_connect:Waiting to connected.");
		sleep(reconnect_time);
		if(ctx->Connected == 0) {
			MQTT_connect(ctx);
			if (reconnect_time < 5) {
				reconnect_time ++;
			}
		}		
	}
	
	while(running) {
		WiringPinMonitor(ctx);
		if(ctx->Connected == 0) {
			sleep(reconnect_time);
			if (!MQTTAsync_isConnected(ctx->client)) {
				MQTT_connect(ctx);
				if (reconnect_time < 5) {
					reconnect_time ++;
				}
			}		
		} else {
			reconnect_time = RECONNECT_TIME;
		}
		usleep(20 * 10000);
	}

	MQTT_cancel_subs(ctx, pinConf, NumPins);
	MQTT_Disconnect(ctx);
	MQTT_destroy(&ctx);
	log_close();
	if (daemon && pid_file_name != NULL) free(pid_file_name);
	
	return EXIT_SUCCESS;
}


static void daemonize() {
		
	pid_t pid = 0;
	int fd;

	pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* The child process becomes leader */
	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	/* Ignore signal sent from child to parent process */
	signal(SIGCHLD, SIG_IGN);

	/* Fork off for the second time*/
	pid = fork();


	if (pid < 0) {
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0);
	chdir("/");

	/* Close all open file descriptors */
	for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
		close(fd);
	}

	/* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
	stdin = fopen("/dev/null", "r");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");

	if(pid_file_name == NULL ) {
		pid_file_name = PID_FILE;
	}
	
	/* Try to write PID of daemon to lockfile */
	if (pid_file_name != NULL)
	{
		char str[256];
		pid_fd = open(pid_file_name, O_RDWR|O_CREAT, 0640);
		if (pid_fd < 0) {
			log_msg(LOG_ERR,"Failed set pid");
			exit(EXIT_FAILURE);
		}		
		if (lockf(pid_fd, F_TLOCK, 0) < 0) {		
			log_msg(LOG_ERR,"Failed lock pid");
			exit(EXIT_FAILURE);
		}
		
		/* Get current PID */
		sprintf(str, "%d\n", getpid());
		/* Write PID to lockfile */
		write(pid_fd, str, strlen(str));
	}	
}

void sig_handler(int sig)
{
	if (sig == SIGINT) {
		log_msg(LOG_DEBUG, "Stopping domo daemon");
		if (pid_fd != -1) {
			lockf(pid_fd, F_ULOCK, 0);
			close(pid_fd);
		}
		/* Try to delete lockfile */
		if (pid_file_name != NULL) {
			unlink(pid_file_name);
		}
		running = 0;
		/* Reset signal handling to default behavior */
		signal(SIGINT, SIG_DFL);				
	} else if (sig == SIGHUP) {
		/* TODO */
		log_msg(LOG_DEBUG, "TODO: Reloading domo daemon");
		
	} else if (sig == SIGCHLD) {
		log_msg(LOG_DEBUG, "Domo daemon recive SIGCHLD signal");
	}
}