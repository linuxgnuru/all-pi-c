import time
import sys
import eyed3
import os.path

def GetTag(songfile):
        audiofile = eyed3.load(songfile)
        artist = audiofile.tag.artist
        album = audiofile.tag.album
        title = audiofile.tag.title
        tmptrack_num = audiofile.tag.track_num
        track_num_a = tmptrack_num[0]
        track_num_b = tmptrack_num[1]
        if track_num_a:
            if track_num_b:
                track_num = str(track_num_a) + '/' + str(track_num_b)
            else:
                track_num = str(track_num_a)
        else:
            track_num = ''
        print 'Title: %s\nArtist: %s\tAlbum: %s\nTrack: %s' % (title, artist, album, track_num)

agv_num = len(sys.argv)

if agv_num == 1:
    sys.exit("usage: " + sys.argv[0] + " text")
for i in range(1, agv_num):
    tmpthing = sys.argv[i]
    if os.path.isfile(tmpthing):
        GetTag(tmpthing)
        print ''
    else:
        if os.path.isdir(tmpthing):
            for subdir, dirs, files in os.walk(tmpthing):
                for file in files:
                    GetTag(os.path.join(subdir, file))
                    print ''
        else:
            print '%s is not a vaild song' % (tmpthing)
