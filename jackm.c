/*************************************************************************
 * This file is part of jackmono
 * github.con/univrsal/jackmono
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "wildcard.h"
#include "stack.h"
#include "util.h"

#include <jack/jack.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

bool process_status(int stat)
{
	if (stat & JackFailure) {
		if (stat & JackInitFailure)
			printf("Exited with JackInitFailure\n");
		if (stat & JackInvalidOption)
			printf("Exited with JackInvalidOption\n");
		if (stat & JackNameNotUnique)
			printf("JackT is already runnning!\n");
		if (stat & JackServerFailed)
			printf("Exited with JackServerFailed\n");
		if (stat & JackNoSuchClient)
			printf("Exited with JackNoSuchClient\n");
		if (stat & JackLoadFailure)
			printf("Exited with JackLoadFailure\n");
		if (stat & JackShmFailure)
			printf("Exited with JackShmFailure\n");
		if (stat & JackVersionError)
			printf("Exited with JackVersionError\n");
		if (stat & JackBackendError)
			printf("Exited with JackBackendError\n");
		if (stat & JackClientZombie)
			printf("Exited with JackClientZombie\n");

		return false;
	} else if (stat & JackServerStarted) {
		printf("Successfuly connected to JACK server\n");
		return true;
	} else if (stat == 0)
		return true;
	return false;
}

static volatile sig_atomic_t flag = 1;
static libs_stack_t *stack = NULL;
static pthread_mutex_t register_mutex;

struct cb_data {
	jack_client_t *client;
	const char *wildcard;
	const char *target;
	volatile bool running;
};

void register_callback(jack_port_id_t port, int reg, void *arg)
{
	/* check if port is an output and if it has only one port
	 * then check if that is connected to the system output 1 and 2
	 * if not connect it */
	struct cb_data *d = arg;
	if (!reg || !d->running || !d->client)
		return;
	jack_port_t *p = jack_port_by_id(d->client, port);
	if (!p || jack_port_flags(p) & JackPortIsInput)
		return;

	const char *name = jack_port_short_name(p);
	if (name && wc_matches(name, d->wildcard)) {
		pthread_mutex_lock(&register_mutex);
		printf("Queued connection...\n");
		libs_stack_push(stack, &port);
		pthread_mutex_unlock(&register_mutex);
	}

}

void shutdown_callback(void *arg)
{
	/* Shutdown this progam */
	printf("Received shutdown callback\n");
	struct cb_data *d = arg;
	d->running = false;
	d->client = NULL; /* Will be free'd at the end of the program */
}

void signal_handler(int sig)
{
	flag = 0;
}

int main(int argc, const char* argv[])
{
	struct cb_data d;
	d.client = NULL;
	d.running = true;
	jack_status_t status;
	jack_client_t *client = jack_client_open("jackt", JackNoStartServer |
	                                         JackUseExactName, &status);
	pthread_mutex_init(&register_mutex, NULL);
	stack = libs_stack_create(32, sizeof(jack_port_id_t));

	if (!client) {
		printf("Couldn't open local JACK server!\n");
		goto end;
	}

	/* Prepare callback data */
	d.client = client;
	d.running = true;

	if (argc > 1)
		d.wildcard = argv[1];
	else
		d.wildcard = "*";

	if (argc > 2)
		d.target = argv[2];
	else
		d.target = "system:playback_2";

	if (!process_status((int)status))
		goto free;

	/* Listen for shutdowns */
	jack_on_shutdown(client, shutdown_callback, &d);
	signal(SIGINT, signal_handler);

	/* Listen for new ports */
	if (jack_set_port_registration_callback(client, register_callback, &d) != 0) {
		printf("Couldn't register register_callback! Exiting\n");
		goto free;
	}

	if (jack_activate(client) != 0) {
		printf("Couldn't active client!\n");
		goto free;
	}

	/* Potentialy check existing ports for mono etc. */
	//const char** ports = jack_get_ports(client, NULL, NULL, JackPortIsOutput);

	while (flag) {
		pthread_mutex_lock(&register_mutex);
		if (libs_stack_count(stack) == 1) {
			/* We only got one matching port this cycle -> assume this is mono */
			jack_port_id_t *id = libs_stack_top(stack);
			jack_port_t *p = jack_port_by_id(client, *id);
			const char *n = jack_port_name(p);
			libs_stack_pop(stack);
			int r = jack_connect(client, n, d.target);
			if (r == 0) {
				printf("Connected!\n");
			}
		} else if (libs_stack_count(stack) > 1){
			printf("Got more than one port (%i)\n", libs_stack_count(stack));
			/* We got more than one port -> assume it's stereo and dump them */
			while (!libs_stack_empty(stack))
				libs_stack_pop(stack);
		}
		pthread_mutex_unlock(&register_mutex);
		util_msleep(500);
	}

free:
	if (client)
		jack_client_close(client);
end:
	while (!libs_stack_empty(stack))
		libs_stack_pop(stack);
	libs_stack_destroy(stack);
	return 0;
}
