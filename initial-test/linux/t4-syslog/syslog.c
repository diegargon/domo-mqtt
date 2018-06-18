/*

*/

/*
PARAMETERS
LOG_CONS Write directly to system console if there is an error while sending to system logger.
LOG_NDELAY Open the connection immediately (normally, the connection is opened when the first message is logged). LOG_NOWAIT Don't wait for child processes that may have been created while logging the message. (The GNU C library does not create a child process, so this option has no effect on Linux.)
LOG_ODELAY The converse of LOG_NDELAY; opening of the connection is delayed until syslog() is called. (This is the default, and need not be specified.)
LOG_PERROR (Not in SUSv3.) Print to stderr as well. LOG_PID Include PID with each message.

FACILITY
LOG_AUTHPRIV security/authorization messages (private) LOG_CRON clock daemon (cron and at)
LOG_DAEMON system daemons without separate facility value LOG_FTP ftp daemon LOG_KERN kernel messages
LOG_LOCAL0 through LOG_LOCAL7 reserved for local use
LOG_LPR line printer subsystem LOG_MAIL mail subsystem LOG_NEWS USENET news subsystem
LOG_SYSLOG messages generated internally by syslogd
LOG_USER (default) generic user-level messages
LOG_UUCP UUCP subsystem

LEVELS
LOG_EMERG system is unusable
LOG_ALERT action must be taken immediately
LOG_CRIT critical conditions
LOG_ERR error conditions LOG_WARNING warning conditions LOG_NOTICE normal, but significant, condition
LOG_INFO informational message
LOG_DEBUG debug-level message
*/


#include <unistd.h> // for getuid
#include <syslog.h>

int main(void) {
	//FILTER
	//setlogmask (LOG_UPTO (LOG_NOTICE));

	openlog ("example", LOG_PID | LOG_NDELAY, LOG_DAEMON);

	syslog (LOG_NOTICE, "Program started by User %d", getuid ());
	syslog (LOG_INFO, "A tree falls in a forest");
	syslog (LOG_DEBUG, "in DEBUG mode");
	syslog (LOG_ERR, "dropping a error log");
	closelog ();
}