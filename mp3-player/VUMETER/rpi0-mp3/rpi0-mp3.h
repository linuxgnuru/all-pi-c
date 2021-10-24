/*
 * header file for lcd-mp3.c
 *
 * John Wiggins
 */

// for thread
#include <pthread.h>
// for mp3 playing
#include <ao/ao.h>
#include <mpg123.h>
// for id3
#include <sys/types.h>

// # defines:
#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

#define MAXDATALEN 256

// Mount enum
typedef enum {
	UNMOUNTED,
	FILES_OK,
	MOUNT_ERROR,
	NO_FILES
} filestatus_enum;

typedef enum {
	PLAY,
	PREV,
	NEXT,
	PAUSE,
	STOP,
	SHUFFLE,
	QUIT
} status_enum;

// playlist
typedef struct playlist_node {
  int index;
  void *songptr;
  struct playlist_node *nextptr;
} playlist_node_t;

typedef playlist_node_t *playlist_t;

struct song_info {
	char base_filename[MAXDATALEN];
	char filename[MAXDATALEN];
	int song_number;
	int song_over;
	int play_status;
	pthread_mutex_t pauseMutex;
	pthread_mutex_t writeMutex;
	pthread_cond_t m_resumeCond;
}; struct song_info cur_song;

// TODO use this somewhere... because right now it's not being used.
struct play_status {
	int is_playing;
	int is_paused;
	int is_stopped;
	int song_over;
}; struct play_status cur_status;

// Global vars
int num_songs; // current number of songs.

