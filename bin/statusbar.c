#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <alsa/asoundlib.h>

char *stat_batt()
{
	FILE *f;
	char *batt_now;
	char *batt_max;
	char *batt_state;
	double batt_percent;
	char *fmt;
	f = fopen("/sys/class/power_supply/BAT0/energy_now", "r");
	batt_now = malloc(9);
	fgets(batt_now, 9, f);
	fclose(f);
	f = fopen("/sys/class/power_supply/BAT0/energy_full", "r");
	batt_max = malloc(9);
	fgets(batt_max, 9, f);
	fclose(f);
	f = fopen("/sys/class/power_supply/BAT0/status", "r");
	batt_state = malloc(14);
	fgets(batt_state, 14, f);
	batt_state[strlen(batt_state)-1] = 0; // Remove newline
	fclose(f);
	batt_percent = (double)atoi(batt_now) / atoi(batt_max) * 100;
	fmt = malloc(64);
	sprintf(fmt, "%i%% (%s)", (int)round(batt_percent), batt_state);
	return fmt;
}

bool earbuds_on()
{
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	snd_mixer_elem_t *elem;
	long value;
	long min;
	long max;
	long percent;
	char *fmt;
	bool earbuds = false;
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "bluealsa");
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);
	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	elem = snd_mixer_first_elem(handle);
	if (elem)
		return true;
	return false;
}

char *stat_vol()
{
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	snd_mixer_elem_t *elem;
	long value;
	long min;
	long max;
	long percent;
	char *fmt;
	bool earbuds = earbuds_on();
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "bluealsa");
	snd_mixer_attach(handle, "default");
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);
	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	if (earbuds == false) {
		snd_mixer_selem_id_set_name(sid, "Master");
	} else {
		snd_mixer_selem_id_set_name(sid, "Push - A2DP");
	}
	elem = snd_mixer_find_selem(handle, sid);
	snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &value);
	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	percent = ((double)value - (double)min) / ((double)max - (double)min) * 100;
	fmt = malloc(5);
	sprintf(fmt, "%i%%", (int)round(percent));
	return fmt;
}

char *get_value(char *line) {
	char *value;
	value = malloc(strlen(line) - 3);
	strncpy(value, line, strlen(line) - 4);
	value[strlen(line) - 4] = 0;
	value = strrchr(value, ' ');
	return value + 1;
}

char *stat_mem()
{
	FILE *f;
	size_t line_len = 28;
	char *line;
	char *total;
	char *free;
	double percent;
	char *fmt;
	f = fopen("/proc/meminfo", "r");
	line = malloc(line_len);
	while (getline(&line, &line_len, f) != -1) {
		if (memcmp(line, "MemTotal", 8) == 0)
			total = get_value(line);
		if (memcmp(line, "MemFree", 7) == 0)
			free = get_value(line);
	}
	fclose(f);
	fmt = malloc(5);
	percent = ((double)atoi(total) - (double)atoi(free)) / (double)atoi(total) * 100;
	sprintf(fmt, "%i%%", (int)round(percent));
	return fmt;
}

char *stat_bright()
{
	FILE *f;
	char *value;
	char *max;
	double percent;
	char *fmt;
	f = fopen("/sys/class/backlight/intel_backlight/brightness", "r");
	value = malloc(5);
	fgets(value, 5, f);
	fclose(f);
	f = fopen("/sys/class/backlight/intel_backlight/max_brightness", "r");
	max = malloc(5);
	fgets(max, 5, f);
	fclose(f);
	fmt = malloc(5);
	percent = (double)atoi(value) / atoi(max) * 100;
	sprintf(fmt, "%i%%", (int)round(percent));
	return fmt;
}

char *stat_date()
{
	time_t raw;
	struct tm *info;
	char *fmt;
	time(&raw);
	info = localtime(&raw);
	fmt = malloc(11);
	strftime(fmt, 11, "%Y-%m-%d", info);
	return fmt;
}

char *stat_time()
{
	time_t raw;
	struct tm *info;
	char *fmt;
	time(&raw);
	info = localtime(&raw);
	fmt = malloc(9);
	strftime(fmt, 9, "%X", info);
	return fmt;
}

int main()
{
	printf("Battery: %s | Volume: %s | Memory: %s | Brightness: %s | Date: %s | Time: %s\n",
	stat_batt(), stat_vol(), stat_mem(), stat_bright(), stat_date(),
	stat_time());
	return 0;
}
