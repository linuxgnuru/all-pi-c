#!/usr/bin/python3
# -*- coding: utf-8 -*-
# Do not forget to run "mpc update" before using other controls.
import lirc
import os
import subprocess
import sys
import time

USER = 'pi'
BASE_DIR = os.path.dirname(os.path.abspath(os.path.expanduser(__file__)))


class MopidyController():
    CONF_DIR = BASE_DIR
    PID_DIR = BASE_DIR

    def execute(self, cmd):
        p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
        out, err = p.communicate()
        out = out.decode('utf-8') if out else ''
        if err:
            if out:
                out += '\n'
            out += 'Stderr: ' + err.decode('utf-8')
        logger.debug('>>> %s', cmd)
        logger.debug('code: %s | %s', p.returncode, out)
        return p.returncode == 0, out

        lirc.init('mpc_lirc', os.path.join(BASE_DIR, 'lircrc.conf'))
        while True:
            try:
                received = lirc.nextcode()
            except Exception as e:
                time.sleep(3)
            else:
                if received:
                    self.key_pressed(received[0])

    def key_pressed(self, key):
        if key == 'KEY_PLAYPAUSE':
            self.execute(['/usr/bin/mpc', 'toggle'])
        elif key == 'KEY_VOLUMEUP':
            self.execute(['/usr/bin/mpc', 'volume', '+5'])
        elif key == 'KEY_VOLUMEDOWN':
            self.execute(['/usr/bin/mpc', 'volume', '-5'])
        elif key == 'KEY_PREVIOUS':
            self.execute(['/usr/bin/mpc', 'prev'])
        elif key == 'KEY_NEXT':
            self.execute(['/usr/bin/mpc', 'next'])
        elif key == 'KEY_F1':
#do something
        elif key == 'KEY_F2':
#do something
        elif key == 'KEY_CHANNEL':
            self.execute(['/usr/bin/mpc', 'shuffle'])
        elif key == 'KEY_MODE':
            self.execute(['sudo', 'shutdown', '-h', 'now'])
            lirc.deinit()
            sys.exit(0)
        elif key == 'KEY_CHANNELDOWN':
#do something
        elif key == 'KEY_CHANNELUP':
#do something
        elif key in ('KEY_0', 'KEY_1', 'KEY_2', 'KEY_3', 'KEY_4', 'KEY_5', 'KEY_6', 'KEY_7', 'KEY_8', 'KEY_9'):
#do something
        else:
#do something


if __name__ == '__main__':
    user = subprocess.getoutput('whoami')
    if user != USER:
        # switch from root to requested user
        print('Using user %s.' % USER)

    MopidyController()
