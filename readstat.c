/*
 * =====================================================================================
 *
 *       Filename:  readstat.c
 *
 *    Description:  a demonstration of FIFO system call
 *
 *        Version:  1.0
 *        Created:  27/12/12 21:07:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zex (zex), top_zlynch@yahoo.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>

jmp_buf cur_pos;
int running = 0;
int ori_quit;

void print_stat (struct stat *st)
{
  printf ("st_dev: %u\n"
		"st_ino: %u\n"
		"st_mode: %u\n"
		"st_nlink: %u\n"
		"st_uid: %u\n"
		"st_gid: %u\n"
		"st_rdev: %u\n"
		"st_size: %u\n"
		"st_atime: %s\n"
		"st_mtime: %u\n"
		"st_ctime: %u\n"
		"----------------------\n",
		st->st_dev, st->st_ino, st->st_mode,
		st->st_nlink, st->st_uid, st->st_gid, st->st_rdev,
		st->st_size, ctime(&st->st_atime),
		st->st_mtime, st->st_ctime);
}

int main(argc, argv)
	int argc;
	const char *argv[];
{
	struct stat stat0;
	struct stat stdinstat;
	char tmpl0[30] = "genby_readstat_mkdtempXXXXXX";
	char tmpl1[30] = "genby_readstat_mkstempXXXXXX";
	char tmpl2[40] = "genby_readstat_mkstemps_XXXXXX_file";
	int when_int(), when_quit(), when_61();

	link("a", "b");

	stat ("a", &stat0);
	print_stat(&stat0);
	fstat (STDIN_FILENO, &stdinstat);
	print_stat(&stdinstat);

	mkfifo("./c", 0);
	//pause(); // pause the procedure to check out c

	unlink("c");

	// 1. tmpl must be a char array
	// 2. the last six letters must be XXXXXX
	if (!mkdtemp(tmpl0))
		perror("mkdtemp");

	if (mkstemp(tmpl1) == -1)
		perror("mkstemp");
	
	if (mkstemps(tmpl2, 5) == -1)
		perror("mkstemps");
	
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, when_int);

	ori_quit = signal(SIGQUIT, when_quit);
	signal(61, when_61);

	running = 1;
	setjmp(cur_pos); // save current statck position

	printf("Enter looping...\n");
//	running = 0;

	while(running) {
		printf("looping...\n");
		sleep(3);
	}
	
	printf("leave loop\n");
	return 0;
}

int when_int(int a)
{
	signal(SIGINT, when_int); // reset for next interrupt
	printf("interrupt occured\n");
	longjmp(cur_pos, 0); // return to saved state
}

int when_quit(int a)
{
	printf("%s\n", __func__);
	signal(SIGQUIT, ori_quit);
	printf("SIGQUIT is restored, try SIGQUIT again\n");
//	running = 0;
	return 0;
}

int when_61()
{
	printf("%s\n", __func__);
	running = 0;
	return 0;
}
