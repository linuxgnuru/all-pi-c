/*
 *  rpi0-mp3
 *
 *  John Wiggins (jcwiggi@gmail.com)
 *
 * NOTE: I have changed from Makefile to cmake.  In order to make this, run the following:

 cmake -H. -Bbuild

 and then run:

 cmake --build build --


 *  mp3 player for Raspberry Pi zero with buttons for control
 * 
 *
 *  Portions of this code were borrowed from MANY other projects including (but not limited to)
 *
 *  - wiringPi example code http://wiringpi.com/
 *    - Many thanks to Gordon for making the wiringPi library.
 *
 *  - The rotary encoder and volume control was borrowed from the MPD player found here:
 *    - http://theatticlight.net/posts/My-Embedded-Music-Player-and-Sound-Server
 *
 *  - http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html
 *
 *  - http://www.arduino.cc/en/Tutorial/Debounce
 *
 *  - Many thanks to those who helped me out at StackExchange
 *    (http://raspberrypi.stackexchange.com/)
 *
 *  Known issues:
 *
 *  - The MP3 decoding part I use for some reason always gives me an error on STDERR and I haven't the
 *    time to go through the lib sources to try to find out what's going on; so I always just run the
 *    program with 2>/dev/null (e.g. mp3-usb 2>/dev/null)
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 *  Requires:
 *  - Program must be run as root or type "sudo chmod +s [executable]"
 *  - Libraries:
 *    pthread
 *    libao-dev
 *    libmpg123-dev
 *    libasound2
 *    (all the above are available via apt-get if using raspbian)
 *
 *    wiringPi (available via: git clone git://git.drogon.net/wiringPi )
 *  - System setup:
 *    - A directory /MUSIC needs to be created.
 *    (optional as the program will attempt to mount the flash)
 *    - In the file, /etc/fstab, the following entry needs to be added so the usb flash will be mounted:
 *
 *    /dev/sda1       /MUSIC          vfat    defaults          0       2
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <signal.h>

#include <ncurses.h>

// For volume control
#include <math.h>
#include <alsa/asoundlib.h>

// For mounting
#include <sys/mount.h>
#include <dirent.h> 

// For subdirectory searching
#include <limits.h>
#include <sys/types.h>

// For wiringPi
#include <wiringPi.h>

#include "rpi0-mp3.h"

// For rotary encoder for volume
#include "rotaryencoder.h"

#define exp10(x) (exp((x) * log(10)))

// --------- BEGIN USER MODIFIABLE VARS ---------

/*
   XXX
   Just some useful information
   +----+----++----+----+
   | WPI BCM || BCM WPI |
   +----+----++----+----+
   |  0 | 17 ||  4 | 7  |
   |  1 | 18 ||  5 | 21 |
   |  2 | 27 ||  6 | 22 |
   |  3 | 22 || 12 | 26 |
   |  4 | 23 || 13 | 23 |
   |  5 | 24 || 16 | 27 |
   |  6 | 25 || 17 | 0  |
   |  7 | 4  || 18 | 1  |
   | 21 | 5  || 19 | 24 |
   | 22 | 6  || 20 | 28 |
   | 23 | 13 || 21 | 29 |
   | 24 | 19 || 22 | 3  |
   | 25 | 26 || 23 | 4  |
   | 26 | 12 || 24 | 5  |
   | 27 | 16 || 25 | 6  |
   | 28 | 20 || 26 | 25 |
   | 29 | 21 || 27 | 2  |
   +----+----++----+----+
 +-----+-----+----------+-----+-----+
 | BCM | wPi | Physical | wPi | BCM |
 +-----+-----+----++----+-----+-----+
 |   4 |   7 |  7 || 8  | -   | -   |
 |  17 |   0 | 11 || 12 | 1   | 18  |
 |  27 |   2 | 13 || 14 | -   | -   |
 |  22 |   3 | 15 || 16 | 4   | 23  |
 |   - |   - | 17 || 18 | 5   | 24  |
 |   - |   - | 21 || 22 | 6   | 25  |
 |   5 |  21 | 29 || 30 | -   | -   |
 |   6 |  22 | 31 || 32 | 26  | 12  |
 |  13 |  23 | 33 || 34 | -   | -   |
 |  19 |  24 | 35 || 36 | 27  | 16  |
 |  26 |  25 | 37 || 38 | 28  | 20  |
 |   - |   - | 39 || 40 | 29  | 21  |
 +-----+-----+----++----+-----+-----+
 | BCM | wPi | Physical | wPi | BCM |
 +-----+-----+----------+-----+-----+
 */

/*
 * -- NOTE --
 *
 * pins use wiringPi ordering however BCM values are shown
 * in case you wish to use that numbering scheme
 */

/*
 * Button
 */
#define prevButtonPin 23 // BCM 13
#define playButtonPin 22 // BCM 6
#define nextButtonPin 2  // BCM 27
#define shufButtonPin 26 // BCM 12
#define quitButtonPin 1  // BCM 18

// Rotary encoder pins
#define encoderPinB   29 // BCM 21
#define encoderPinA   25 // BCM 26
#define muteButtonPin 28 // BCM 20

/*
 * LED
 */
// yellow led
#define pauseLedPin 7 // BCM 4
// blue led
#define muteLedPin 0  // BCM 17

// --------- END USER MODIFIABLE VARS ---------

/*
 * Debounce tracking stuff
 */

#define BTN_DELAY 30

int playButtonState;
int prevButtonState;
int nextButtonState;
int quitButtonState;
int shufButtonState;
int muteButtonState;

int lastPlayButtonState;
int lastPrevButtonState;
int lastNextButtonState;
int lastQuitButtonState;
int lastShufButtonState;
int lastMuteButtonState;

long lastPlayDebounceTime;
long lastPrevDebounceTime;
long lastNextDebounceTime;
long lastQuitDebounceTime;
long lastShufDebounceTime;
long lastMuteDebounceTime;

long debounceDelay = 50;

const int numButtons = 6;

const int buttonPins[] = {
    playButtonPin,
    prevButtonPin,
    nextButtonPin,
    quitButtonPin,
    shufButtonPin,
    muteButtonPin
};

// More global variables
static int Index = 0;
static playlist_t Tmp_Playlist;
static char card[64] = "hw:0";

snd_mixer_t *handle = NULL;
snd_mixer_elem_t *elem = NULL;

// System stuff
// Error stuff
int printErr(char *msg, char *f, int l)
{
    fprintf(stderr, "[%s - %d]: %s\n", f, l, msg);
    return 0;
}

char *printFlag(int boo) { return (boo == TRUE ? "TRUE" : "FALSE"); }

// For signal catching
static void die(int sig)
{
    // Insert any GPIO cleaning here.
    digitalWrite(pauseLedPin, LOW);
    digitalWrite(muteLedPin, LOW);
    // TODO maybe try to unmount the usb stick or some other clean up here... maybe?
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2)
    {
        (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
        if (handle != NULL)
            snd_mixer_close(handle);
    }
    mpg123_exit();
    ao_shutdown();
    endwin();
    exit(1);
}

/*
 * Volume control!
 *
 * The following code is borrowed from the MPD project.
 * Info can be found here: http://theatticlight.net/posts/My-Embedded-Music-Player-and-Sound-Server
 */

double get_normalized_volume(snd_mixer_elem_t *elem)
{
    long max, min, value;
    int err;

    err = snd_mixer_selem_get_playback_dB_range(elem, &min, &max);
    if (err < 0)
    {
        printErr("Error getting volume", __FILE__, __LINE__);
        return 0;
    }
    err = snd_mixer_selem_get_playback_dB(elem, 0, &value);
    if (err < 0)
    {
        printErr("Error getting volume", __FILE__, __LINE__);
        return 0;
    }
    // Perceived 'loudness' does not scale linearly with the actual decible level
    // it scales logarithmically
    return exp10((value - max) / 6000.0);
}

// Set the volume from a floating point number 0..1
void set_normalized_volume(snd_mixer_elem_t *elem, double volume)
{
    long min, max, value;
    int err;

    if (volume < 0.017170)
        volume = 0.017170;
    else if (volume > 1.0)
        volume = 1.0;
    err = snd_mixer_selem_get_playback_dB_range(elem, &min, &max);
    if (err < 0)
    {
        // XXX
        printErr("Error setting volume", __FILE__, __LINE__);
        return;
    }
    // Perceived 'loudness' does not scale linearly with the actual decible level
    // it scales logarithmically
    value = lrint(6000.0 * log10(volume)) + max;
    snd_mixer_selem_set_playback_dB(elem, 0, value, 0);
}

double map(float x, float x0, float x1, float y0, float y1)
{
    float y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    double z = (double)y;
    return z;
}

void print_vol_num(snd_mixer_elem_t *elem)
{
    int volbar_length = rint(get_normalized_volume(elem) * (double)10-1);
    int cur_vol = 0;

    cur_vol = map(volbar_length, -1, 10 - 1, 0, 99);
    move(1, 0);
    printw("volume: %3d", cur_vol);
    refresh();
}

// Print usage
int usage(const char *progName)
{
    fprintf(stderr, "Usage: %s [OPTION] \n"
            "-dir [dir] \n"
            "-songs [MP3 files]\n"
            "-usb (this reads in any music found in /MUSIC)\n",
            progName);
    return EXIT_FAILURE;
}

// Get the extension
const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');

    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

// Mounting function; might use it in the future

// TODO mountToggle is never used so might as well remove this function
// Mount (if cmd == 1, do not attempt to unmount)
int mountToggle(int cmd, char *dir_name)
{
    if (mount("/dev/sda1", dir_name, "vfat", MS_RDONLY | MS_SILENT, "") == -1)
    {
        // If it is already mounted; then unmount it.
        // Use umount2 so we can force the mount.
        if (errno == EBUSY && cmd == 2)
        {
            umount2("/MUSIC", MNT_FORCE);
            return UNMOUNTED;
        }
        // Filesystem is already mounted so just return as mounted.
        else if (errno == EBUSY && cmd != 2)
            return FILES_OK;
        else
            return MOUNT_ERROR;
    }
    else
        return FILES_OK;
}

// Check to see if the USB flash was mounted or not.
// Mount (if cmd == 1, do not attempt to unmount)
int checkMount()
{
    if (mount("/dev/sda1", "/MUSIC", "vfat", MS_RDONLY | MS_SILENT, "") == -1)
    {
        if (errno == EBUSY) // EBUSY means the filesystem is already mounted so just return OK
            return FILES_OK;
        else
            return MOUNT_ERROR;
    }
    else
        return FILES_OK;
}

// Linked list / playlist functions Possible FIXME ... maybe convert this whole program into C++ and make the following a class...?
// TODO also, why is this even returning anything at all?
int playlist_init(playlist_t *playlistptr)
{
    *playlistptr = NULL;
    return 1; // ???? Why?
}

// again; why is this returning anything?
int playlist_add_song(int index, void *songptr, playlist_t *playlistptr)
{
    playlist_node_t *cur, *prev, *new;
    int found = FALSE;

    for (cur = prev = *playlistptr; cur != NULL; prev = cur, cur = cur->nextptr)
    {
        if (cur->index == index)
        {
            free(cur->songptr);
            cur->songptr = songptr;
            found = TRUE;
            break;
        }
        else if (cur->index > index)
            break;
    }
    if (!found)
    {
        new = (playlist_node_t *)malloc(sizeof(playlist_node_t));
        new->index = index;
        new->songptr = songptr;
        new->nextptr = cur;
        if (cur == *playlistptr)
            *playlistptr = new;
        else
            prev->nextptr = new;
    }
    return 1; // WHYWYWYWYWY????
}

// Get song name from playlist with index
int playlist_get_song(int index, void **songptr, playlist_t *playlistptr)
{
    playlist_node_t *cur, *prev;

    // Initialize to "not found"
    *songptr = NULL;
    // Look through index for our entry
    for (cur = prev = *playlistptr; cur != NULL; prev = cur, cur = cur->nextptr)
    {
        if (cur->index == index)
        {
            *songptr = cur->songptr;
            break;
        }
        else if (cur->index > index)
            break;
    }
    return 1; // WHY ARE YOU RETURNING ANYTHING?
}

// Creates playlist

// NOTE: Brand new! Now we read in sub directories!!

// Recursive function to enter sub directories
void list_dir(const char *dir_name)
{
    DIR *d;
    char *basec, *bname;
    char *string;

    d = opendir(dir_name);
    if (!d)
    {
        fprintf(stderr, "[%s - %d]: Cannot open directory '%s': %s\n", __FILE__, __LINE__, dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        struct dirent *dir;
        const char *d_name;

        dir = readdir(d);
        // There are no more entries in this directory, so break out of the while loop.
        if (!dir)
            break;
        d_name = dir->d_name;
        // 8 = normal file; non-directory
        if (dir->d_type == 8)
        {
            string = malloc(MAXDATALEN);
            if (string == NULL)
                perror("malloc: reReadPlaylist");
            strcpy(string, dir_name);
            strcat(string, "/");
            strcat(string, d_name);
            // Get just the filename, strip the path info
            basec = strdup(string);
            bname = basename(basec);
            // Make sure we only add mp3 files
            if (strcasecmp(get_filename_ext(bname), "mp3") == 0)
                playlist_add_song(Index++, string, &Tmp_Playlist); // FIXME I REALLY hate having to use global variables...
        }
        if (dir->d_type & DT_DIR)
        {
            // Check that the directory is not "d" or d's parent.
            if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0)
            {
                int path_length;
                char path[PATH_MAX];

                path_length = snprintf(path, PATH_MAX, "%s/%s", dir_name, d_name);
                if (path_length >= PATH_MAX)
                {
                    fprintf(stderr, "[%s - %d]: Path length has become too long.\n", __FILE__, __LINE__);
                    exit(EXIT_FAILURE);
                }
                list_dir(path); // Recursively call "list_dir" with the new path.
            }
        } // end if dir
    } // end while
    if (closedir(d))
    {
        fprintf(stderr, "[%s - %d]: Could not close '%s': %s\n", __FILE__, __LINE__, dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

// Create the playlist; NOTE Now we read in sub directories...
// Tmp_Playlist and Index are global vars...
playlist_t reReadPlaylist(char *dir_name)
{
    playlist_t new_playlist;

    playlist_init(&new_playlist);
    playlist_init(&Tmp_Playlist);
    Index = 0;
    list_dir(dir_name);
    new_playlist = Tmp_Playlist;
    playlist_init(&Tmp_Playlist);
    pthread_mutex_lock(&cur_song.pauseMutex);
    num_songs = Index - 1;
    pthread_mutex_unlock(&cur_song.pauseMutex);
    return new_playlist;
}

// Convert linked list into array so it can be shuffled
void ll2array(playlist_t *playlistptr, char **arr)
{
    int i;
    playlist_node_t *cur;

    for (i = 0, cur = *playlistptr; cur != NULL; i++, cur = cur->nextptr)
    {
        if (cur->songptr != NULL)
            arr[i] = cur->songptr;
    }
}

// Shuffle / randomize playlist
playlist_t randomize(playlist_t cur_playlist)
{
    char *tmp[MAXDATALEN];
    char *string;
    playlist_t new_playlist;
    int index = 0;

    ll2array(&cur_playlist, tmp);
    srand((unsigned)time(NULL));
    if (num_songs > 1)
    {
        size_t i;
        for (i = 0; i < num_songs - 1; i++)
        {
            if (strlen(tmp[i]) > 3)
            {
                size_t j = i + rand() / (RAND_MAX / (num_songs - i) + 1);
                char *t = tmp[j];
                tmp[j] = tmp[i];
                tmp[i] = t;
            }
        }
    }
    // Convert array back into linked list.
    playlist_init(&new_playlist);
    index = 0;
    while (index < num_songs)
    {
        string = malloc(MAXDATALEN);
        if (string == NULL)
            perror("malloc: shuffle");
        strcpy(string, tmp[index]);
        playlist_add_song(index, string, &new_playlist);
        index++;
    }
    return new_playlist;
}

// Threading functions Functions for when buttons are pressed
void nextSong()
{
    pthread_mutex_lock(&cur_song.pauseMutex);
    cur_song.play_status = NEXT;
    cur_song.song_over = TRUE;
    pthread_mutex_unlock(&cur_song.pauseMutex);
}

void prevSong()
{
    pthread_mutex_lock(&cur_song.pauseMutex);
    cur_song.play_status = PREV;
    cur_song.song_over = TRUE;
    pthread_mutex_unlock(&cur_song.pauseMutex);
}

void shuffleMe()
{
    pthread_mutex_lock(&cur_song.pauseMutex);
    cur_song.play_status = SHUFFLE;
    cur_song.song_over = TRUE;
    pthread_mutex_unlock(&cur_song.pauseMutex);
}

void quitMe()
{
    pthread_mutex_lock(&cur_song.pauseMutex);
    cur_song.play_status = QUIT;
    cur_song.song_over = TRUE;
    pthread_mutex_unlock(&cur_song.pauseMutex);
}

void pauseMe()
{
    pthread_mutex_lock(&cur_song.pauseMutex);
    // Turn on LED showing song is paused
    digitalWrite(pauseLedPin, HIGH);
    cur_song.play_status = PAUSE;
    pthread_mutex_unlock(&cur_song.pauseMutex);
}


void playMe()
{
    pthread_mutex_lock(&cur_song.pauseMutex);
    // Turn on LED showing song is un-paused
    digitalWrite(pauseLedPin, LOW);
    cur_song.play_status = PLAY;
    pthread_cond_broadcast(&cur_song.m_resumeCond);
    pthread_mutex_unlock(&cur_song.pauseMutex);
}

void checkPause()
{
    pthread_mutex_lock(&cur_song.pauseMutex);
    while (cur_song.play_status == PAUSE)
        pthread_cond_wait(&cur_song.m_resumeCond, &cur_song.pauseMutex);
    pthread_mutex_unlock(&cur_song.pauseMutex);
}

// The actual thing that plays the song
void play_song(void *arguments)
{
    struct song_info *args = (struct song_info *)arguments;
    mpg123_handle *mh;
    mpg123_pars *mpar;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;
    int retval;

    ao_device *dev;
    ao_sample_format format;
    int channels, encoding;
    long rate;

    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
    // Try to not show error messages
    mh = mpg123_new(NULL, &err);
    mpar = mpg123_new_pars(&err);
    mpg123_par(mpar, MPG123_ADD_FLAGS, MPG123_QUIET, 0);
    mh = mpg123_parnew(mpar, NULL, &err);
    // Open the file and get the decoding format
    mpg123_open(mh, args->filename);
    mpg123_getformat(mh, &rate, &channels, &encoding);
    // Set the output format and open the output device
    format.bits = mpg123_encsize(encoding) * 8;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(ao_default_driver_id(), &format, NULL);
    // Decode and play
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
    {
        checkPause();
        ao_play(dev, (char *) buffer, done);
        // Stop playing if the user pressed quit, shuffle, next, or prev buttons
        if (cur_song.play_status == QUIT || cur_song.play_status == NEXT || cur_song.play_status == PREV || cur_song.play_status == SHUFFLE)
            break;
    }
    // Clean up
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    pthread_mutex_lock(&(cur_song.writeMutex));
    args->song_over = TRUE;
    // Only set the status to play if the song finished normally
    if (cur_song.play_status != QUIT && cur_song.play_status != SHUFFLE && cur_song.play_status != NEXT && cur_song.play_status != PREV)
        args->play_status = PLAY;
    // FIXME only time cur_status is used?! Might just delete the entire struct...
    cur_status.song_over = TRUE;
    pthread_mutex_unlock(&(cur_song.writeMutex));
}

// Main function
int main(int argc, char **argv)
{
    pthread_t song_thread;
    playlist_t init_playlist;
    playlist_t cur_playlist;
    char *basec, *bname;
    char *string;
    int ival; // for mute
    int index;
    int song_index;
    int i;
    //int number = 0;
    _Bool reading;
    // Flags
#if 0
    _Bool flipflopflag;
    _Bool lastStatus;
    _Bool currentStatus;
#endif
    _Bool haltFlag = FALSE;
    _Bool shuffFlag = FALSE;
    _Bool playlistStatusErr = FILES_OK;

    // Initializations
    playlist_init(&cur_playlist);
    playlist_init(&init_playlist);
    cur_song.song_over = FALSE;
    lastPlayButtonState = lastPrevButtonState = lastNextButtonState = HIGH;
    lastQuitButtonState = lastShufButtonState = lastMuteButtonState = HIGH;
    lastPlayDebounceTime = lastPrevDebounceTime = 0;
    lastNextDebounceTime = lastQuitDebounceTime = 0;
    lastShufDebounceTime = lastMuteDebounceTime = 0;
    // Use the following instead of delay
    if (argc > 1)
    {
        // Random/shuffle songs on startup
        if (strcmp(argv[1], "--songs") == 0)
        {
            for (index = 2; index < argc; index++)
            {
                string = malloc(MAXDATALEN);
                if (string == NULL)
                    perror("malloc: --songs");
                strcpy(string, argv[index]);
                playlist_add_song(index - 1, string, &init_playlist);
                num_songs = argc - 2;
            }
            // FIXME I'm lazy right now; just threw this in so the test at the end won't fail.
            playlistStatusErr = FILES_OK;
        }
        else if (strcmp(argv[1], "--usb") == 0)
        {
            // Check if USB is mounted.
            playlistStatusErr = checkMount();
            if (playlistStatusErr != MOUNT_ERROR)
            {
                if (playlistStatusErr == FILES_OK)
                    init_playlist = reReadPlaylist("/MUSIC");
                if (num_songs == 0)
                    playlistStatusErr = NO_FILES;
            }
        }
        else if (strcmp(argv[1], "--dir") == 0)
        {
            init_playlist = reReadPlaylist(argv[2]);
            if (num_songs == 0)
            {
                fprintf(stderr, "[%s - %d]: No songs found in directory %s\n", __FILE__, __LINE__, argv[2]);
                return -1;
            }
            // FIXME I'm lazy right now; just threw this in so the test at the end won't fail.
            playlistStatusErr = FILES_OK;
        }
    }
    else
    {
      usage(argv[0]);
      return EXIT_FAILURE;
    }
    // Catch signals
    // NOTE: We're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s - %d]: %s\n", __FILE__, __LINE__, strerror(errno));
        return 1;
    }
    // Setup buttons
    for (i = 0; i < numButtons; i++)
    {
        pinMode(buttonPins[i], INPUT);
        pullUpDnControl(buttonPins[i], PUD_UP);
    }
    // Setup our priority
    piHiPri(99);
    // LEDs
    pinMode(muteLedPin, OUTPUT);
    pinMode(pauseLedPin, OUTPUT);
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("mp3 player");
    refresh();
    // Initialize audio
    ao_initialize();
    mpg123_init();
    // Setup volume control
    struct encoder *vol_selector = setupencoder(encoderPinA, encoderPinB);
    if (vol_selector == NULL)
        exit(1);
    int oldvalue = vol_selector->value;
    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "PCM");
    if (snd_mixer_open(&handle, 0) < 0)
    {
        printErr("Error openning mixer", __FILE__, __LINE__);
        exit(1);
    }
    if (snd_mixer_attach(handle, card) < 0)
    {
        printErr("Error attaching mixer", __FILE__, __LINE__);
        snd_mixer_close(handle);
        exit(1);
    }
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0)
    {
        printErr("Error registering mixer", __FILE__, __LINE__);
        snd_mixer_close(handle);
        exit(1);
    }
    if (snd_mixer_load(handle) < 0)
    {
        printErr("Error loading mixer", __FILE__, __LINE__);
        snd_mixer_close(handle);
        exit(1);
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem)
    {
        printErr("Error finding simple control", __FILE__, __LINE__);
        snd_mixer_close(handle);
        exit(1);
    }
    if (playlistStatusErr == FILES_OK)
    {
        song_index = 1;
        if (shuffFlag == TRUE)
            cur_playlist = randomize(init_playlist);
        else
            cur_playlist = init_playlist;
        cur_song.play_status = PLAY;
        digitalWrite(pauseLedPin, LOW);
        digitalWrite(muteLedPin, LOW);
        while (cur_song.play_status != QUIT)
        {
            // Loop playlist; reset song to begining of list
            if (song_index > num_songs)
                song_index = 1;
            string = malloc(MAXDATALEN);
            if (string == NULL)
                perror("malloc: song string");
            playlist_get_song(song_index, (void **) &string, &cur_playlist);
            if (string != NULL)
            {
                // Play the song as a thread
                pthread_create(&song_thread, NULL, (void *) play_song, (void *) &cur_song);
                // Loop to play the song
                while (cur_song.song_over == FALSE)
                {
                    basec = strdup(string);
                    // Get just the filename, strip the path info and extension
                    bname = basename(basec);
                    strcpy(cur_song.filename, string);
                    strcpy(cur_song.base_filename, bname);
                    move(4, 0);
                    printw("filename: [%s]", cur_song.filename);
                    refresh();
                    // Play / Pause button
                    reading = digitalRead(playButtonPin);
                    if (reading != lastPlayButtonState)
                        lastPlayDebounceTime = millis(); // reset the debouncing timer
                    if ((millis() - lastPlayDebounceTime) > debounceDelay)
                    {
                        if (reading != playButtonState)
                        {
                            playButtonState = reading;
                            if (playButtonState == LOW)
                            {
                                if (cur_song.play_status == PAUSE)
                                {
                                    playMe();
                                }
                                else
                                {
                                    pauseMe();
                                }
                            }
                        }
                    }
                    // Save the reading. Next time through the loop, it'll be the lastButtonState:
                    lastPlayButtonState = reading;
                    // Don't even check to see if the prev/next/info/quit/shuffle buttons
                    // have been pressed if we are in a pause state.
                    if (cur_song.play_status != PAUSE)
                    {
                        // Mute
                        reading = digitalRead(muteButtonPin);
                        if (reading != lastMuteButtonState)
                            lastMuteDebounceTime = millis();
                        if ((millis() - lastMuteDebounceTime) > debounceDelay)
                        {
                            if (reading != muteButtonState)
                            {
                                muteButtonState = reading;
                                if (muteButtonState == LOW)
                                {
                                    move(3, 0);
                                    snd_mixer_selem_get_playback_switch(elem, 0, &ival);
                                    if (ival == 1) // 1 = muted
                                    {
                                        printw("muting on ");
                                        // Turn on mute led
                                        digitalWrite(muteLedPin, HIGH);
                                    }
                                    else
                                    {
                                        printw("muting off");
                                        // Turn off mute led
                                        digitalWrite(muteLedPin, LOW);
                                    }
                                    refresh();
                                    snd_mixer_selem_set_playback_switch(elem, 0, !ival);
                                }
                            }
                        }
                        lastMuteButtonState = reading;
#if 0
                        // Volume using new method
                        lastStatus = digitalRead(encoderPinB);
                        while (!digitalRead(encoderPinA))
                        {
                            currentStatus = digitalRead(encoderPinB);
                            flipflopflag = TRUE;
                        }
                        if (flipflopflag == TRUE)
                        {
                            flipflopflag = FALSE;
                            if (!lastStatus && currentStatus) number++;
                            if (lastStatus && !currentStatus) number--;
                            //if (number < 0) number = 0;
                            //if (number > 10) number = 10;
                            int chn = 0;
                            for (; chn <= SND_MIXER_SCHN_LAST; chn++)
                            {
                                double vol = get_normalized_volume(elem);
                                set_normalized_volume(elem, vol + (number * 0.00065105));
                            }
                        }
#endif
// #if 0
                        // Volume (using rotary encoder)
                        if (oldvalue != vol_selector->value)
                        {
                            int change = vol_selector->value - oldvalue;
                            int chn = 0;
                            for (; chn <= SND_MIXER_SCHN_LAST; chn++)
                            {
                                double vol = get_normalized_volume(elem);
                                set_normalized_volume(elem, vol + (change * 0.00065105));
                            }
                            oldvalue = vol_selector->value;
                        }
// #endif
                        // Previous button
                        reading = digitalRead(prevButtonPin);
                        if (reading != lastPrevButtonState)
                            lastPrevDebounceTime = millis();
                        if ((millis() - lastPrevDebounceTime) > debounceDelay)
                        {
                            if (reading != prevButtonState)
                            {
                                prevButtonState = reading;
                                if (prevButtonState == LOW)
                                {
                                    song_index = (song_index - 1 != 0 ? song_index - 1 : num_songs - 1);
                                    prevSong();
                                }
                            }
                        }
                        lastPrevButtonState = reading;
                        // Next button
                        reading = digitalRead(nextButtonPin);
                        if (reading != lastNextButtonState)
                            lastNextDebounceTime = millis();
                        if ((millis() - lastNextDebounceTime) > debounceDelay)
                        {
                            if (reading != nextButtonState)
                            {
                                nextButtonState = reading;
                                if (nextButtonState == LOW)
                                {
                                    song_index = (song_index + 1 < num_songs ? song_index + 1 : 1);
                                    nextSong();
                                }
                            }
                        }
                        lastNextButtonState = reading;
                        // Quit button
                        reading = digitalRead(quitButtonPin);
                        if (reading != lastQuitButtonState)
                            lastQuitDebounceTime = millis();
                        if ((millis() - lastQuitDebounceTime) > debounceDelay)
                        {
                            if (reading != quitButtonState)
                            {
                                quitButtonState = reading;
                                if (quitButtonState == LOW)
                                    quitMe();
                            }
                        }
                        lastQuitButtonState = reading;
                        // Shuffle button
                        reading = digitalRead(shufButtonPin);
                        if (reading != lastShufButtonState)
                            lastShufDebounceTime = millis();
                        if ((millis() - lastShufDebounceTime) > debounceDelay)
                        {
                            if (reading != shufButtonState)
                            {
                                shufButtonState = reading;
                                if (shufButtonState == LOW)
                                {
                                    // Toggle shuffle state (NOTE: shuffFlag is useless here)
                                    shuffFlag = (shuffFlag == TRUE ? FALSE : TRUE);
                                    // The following function signals to go to next song
                                    // and sets the play status to SHUFFLE
                                    shuffleMe();
                                }
                            }
                        }
                        lastShufButtonState = reading;
                        // TODO if the following is put above, the sound skips ...
                        // FIXME also ... if the following is removed / commented out the song skips ...
                        print_vol_num(elem);
                    } // end ! pause
                } // end while
                if (pthread_join(song_thread, NULL) != 0)
                    perror("join error\n");
            }
            // Increment the song_index if the song is over but the next/prev wasn't hit
            if (cur_song.song_over == TRUE && cur_song.play_status == PLAY)
            {
                pthread_mutex_lock(&cur_song.pauseMutex);
                cur_song.song_over = FALSE;
                pthread_mutex_unlock(&cur_song.pauseMutex);
                song_index++;
            }
            // Reset everything if next, prev, or shuffle buttons were pressed
            else if (cur_song.song_over == TRUE && (cur_song.play_status == NEXT || cur_song.play_status == PREV || cur_song.play_status == SHUFFLE))
            {
                pthread_mutex_lock(&cur_song.pauseMutex);
                if (cur_song.play_status == SHUFFLE)
                {
                    if (shuffFlag == TRUE)
                        cur_playlist = randomize(init_playlist);
                    else
                        cur_playlist = init_playlist;
                    song_index = 1;
                }
                cur_song.play_status = PLAY;
                cur_song.song_over = FALSE;
                pthread_mutex_unlock(&cur_song.pauseMutex);
            }
        }
        // Quit button was pressed
        if (handle != NULL)
            snd_mixer_close(handle);
        // Don't shutdown unless the quit button was pressed.
        if (cur_song.play_status == QUIT)
        {
            move(5, 0);
            printw("Good Bye!");
            refresh();
        }
    }
    else if (playlistStatusErr == MOUNT_ERROR)
    {
        move(5, 0);
        printw("No USB inserted.");
        refresh();
    }
    else if (playlistStatusErr == NO_FILES)
    {
        move(5, 0);
        printw("No songs on USB.");
        refresh();
    }
    // no need to set the status to quit
    //cur_song.play_status = QUIT;
    digitalWrite(pauseLedPin, LOW);
    digitalWrite(muteLedPin, LOW);
    mpg123_exit();
    ao_shutdown();
    endwin();
    return 0;
}
