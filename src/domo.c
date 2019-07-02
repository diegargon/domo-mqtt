/*

	Diego Garcia (diegargon) - diego@envigo.net

*/

#include "domo.h"

static void daemonize(void);

static int running = 0;
static char *pid_file_name = NULL;
static int pid_fd = -1;

int main(int argc, char *argv[]) {
	
	int reconnect_time = RECONNECT_TIME;
	int value, option_index = 0;
	int daemon = 0;
	//unsigned long int SubsNcount = 0;
	
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"daemon", no_argument, 0, 'd'},
		{NULL, 0, 0, 0}
	};	

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
	
	(daemon) ? daemonize() : 0;
	
	/* Signal */
	signal(SIGUSR1, sig_handler);
	signal(SIGINT, sig_handler);
	/**/

	GKeyFile *conf = g_key_file_new ();
	domoCfg_Init(conf);
	log_init(APP_NAME, conf, daemon);	
	
	log_msg(LOG_INFO, "Program %s started by user %d", argv[0], getuid());
		
	int numPins = domCfg_getNumElementsList( conf, "WIRING", "PinConf" );	
	PinConfig pinConf[numPins];
		
		
	WiringInit(conf, pinConf, numPins);
	LogPinConfig(pinConf, numPins);
	
	//SubsNcount = domCfg_getNumElementsList( conf, "MQTT", "SubsTopics" );		
	
	MQTTContext *ctx = g_malloc0(sizeof(struct MQTTContext) + (sizeof(struct PinConfig) * numPins) );
	ctx->numPins = numPins;
	ctx->pinConf = pinConf;
	
	MQTT_Init(ctx, conf);	
	running = 1;
	
	while(running) {		
		if(ctx->connected < 1 || !MQTTAsync_isConnected(ctx->client)) {
			//check if we already send a connect request (set to -1) wait and not send another unless onFailure change to 0			
			(ctx->connected != -1) ? MQTT_connect(ctx) : 0;			
			log_msg(LOG_DEBUG,"MQTT_connect:Waiting to connected (code:%d)", ctx->connected);
			sleep(reconnect_time);
			reconnect_time < MAX_RECONNECT_TIME ? reconnect_time++: reconnect_time;

		} else {
			reconnect_time > RECONNECT_TIME ? reconnect_time = RECONNECT_TIME :0;			
			DomoPinMonitor(ctx);
		}
		
		usleep(20 * 10000);
	}
		
	MQTT_Disconnect(ctx);
	log_close();
	(daemon && pid_file_name != NULL) ? free(pid_file_name) : 0;
	
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
	(setsid() < 0) ? exit(EXIT_FAILURE) : 0;
	

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

	(pid_file_name == NULL ) ? pid_file_name = PID_FILE : 0;	
	
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
