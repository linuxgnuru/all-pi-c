#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <alsa/asoundlib.h>

int main()
{
	snd_hctl_t *hctl;
    int err;

	err = snd_hctl_open(&hctl, "hw:0", 0);
	err = snd_hctl_load(hctl);

	//Now we need to initialise a snd_ctl_elem_id_t structure which alsa can use to find the control we want.
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

	//We must specify the interface and either the numeric id
	snd_ctl_elem_id_set_id(id, 1);

	//or the name of the control.
	snd_ctl_elem_id_set_name(id, "Line Playback Volume");

	//With this initialised we can use it to get a snd_hctl_elem_t object and a snd_ctl_elem_value_t object. The elem is retrieved as follows:
	snd_hctl_elem_t *elem = snd_hctl_find_elem(hctl, id);

	//The elem_value is initialised with the id of the element and the value we want to set like this:
	snd_ctl_elem_value_t *control;
	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);    

	//Finally we are ready to write our values to the mixer control:
	snd_ctl_elem_value_set_integer(control, 0, 66);
	err = snd_hctl_elem_write(elem, control);

	snd_ctl_elem_value_set_integer(control, 1, 77);
	err = snd_hctl_elem_write(elem, control);

	//And if we don't want to do anything else we can close the high level control handle:
	snd_hctl_close(hctl);
	return 0;
}
