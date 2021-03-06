/*****************************************************************************\
** ttbin.h                                                                   **
** TTBIN parsing implementation                                              **
\*****************************************************************************/

#ifndef __TTBIN_H__
#define __TTBIN_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define TAG_FILE_HEADER     (0x20)
#define TAG_STATUS          (0x21)
#define TAG_GPS             (0x22)
#define TAG_HEART_RATE      (0x25)
#define TAG_SUMMARY         (0x27)
#define TAG_TRAINING_SETUP  (0x2d)
#define TAG_LAP             (0x2f)
#define TAG_TREADMILL       (0x32)
#define TAG_SWIM            (0x34)
#define TAG_GOAL_PROGRESS   (0x35)
#define TAG_INTERVAL_SETUP  (0x39)
#define TAG_INTERVAL_START  (0x3a)
#define TAG_INTERVAL_FINISH (0x3b)
#define TAG_RACE_SETUP      (0x3c)
#define TAG_RACE_RESULT     (0x3d)

#define ACTIVITY_RUNNING    (0)
#define ACTIVITY_CYCLING    (1)
#define ACTIVITY_SWIMMING   (2)
#define ACTIVITY_TREADMILL  (7)
#define ACTIVITY_FREESTYLE  (8)

typedef struct
{
    double   latitude;      /* degrees */
    double   longitude;     /* degrees */
    float    elevation;     /* metres, initialised to 0 */
    float    heading;       /* degrees, N = 0, E = 90.00 */
    float    speed;         /* m/s */
    time_t   timestamp;     /* gps time (utc) */
    uint16_t calories;
    float    inc_distance;  /* metres */
    float    cum_distance;  /* metres */
    uint8_t  cycles;        /* steps/strokes/cycles etc. */
} GPS_RECORD;

typedef struct
{
    uint8_t  status;        /* 0 = ready, 1 = active, 2 = paused, 3 = stopped */
    uint8_t  activity;      /* 0 = running, 1 = cycling, 2 = swimming, 7 = treadmill, 8 = freestyle */
    time_t   timestamp;     /* utc time */
} STATUS_RECORD;

typedef struct
{
    time_t   timestamp;     /* utc time */
    float    distance;      /* metres */
    uint16_t calories;
    uint32_t steps;
} TREADMILL_RECORD;

typedef struct
{
    time_t   timestamp;         /* utc time */
    float    total_distance;    /* metres */
    uint32_t strokes;           /* since the last report */
    uint32_t completed_laps;
    uint16_t total_calories;
} SWIM_RECORD;

typedef struct
{
    uint32_t total_time;        /* seconds since activity start */
    float    total_distance;    /* metres */
    uint16_t total_calories;
} LAP_RECORD;

typedef struct
{
    time_t  timestamp;          /* utc time */
    uint8_t heart_rate;         /* bpm */
} HEART_RATE_RECORD;

typedef struct
{
    struct
    {
        float    distance;  /* metres */
        uint32_t duration;  /* seconds */
        char     name[17];  /* always null-terminated */
    } setup;
    struct
    {
        float    distance;  /* metres */
        uint32_t duration;  /* seconds */
        uint16_t calories;
    } result;
} RACE_RECORD;

typedef struct
{
    float    distance;  /* metres */
    uint32_t duration;  /* seconds */
    char     name[17];  /* always null-terminated */
} RACE_SETUP_RECORD;

typedef struct
{
    float    distance;  /* metres */
    uint32_t duration;  /* seconds */
    uint16_t calories;
} RACE_RESULT_RECORD;

typedef struct
{
    uint8_t type;       /* 0 = goal distance, 1 = goal time, 2 = goal calories,
                           3 = zones pace, 4 = zones heart, 6 = race,
                           7 = laps time, 8 = laps distance, 9 = laps manual,
                           11 = zones speed, 12 = intervals */
    float   value_min;  /* metres, seconds, calories, secs/km, km/h, bpm (min for zones) */
    float   max;        /* secs/km, km/h, bpm (only used for zones) */
} TRAINING_SETUP_RECORD;

typedef struct
{
    uint8_t  percent;
    uint32_t value;
} GOAL_PROGRESS_RECORD;

typedef struct
{
    uint8_t  warm_type; /* 0 = distance, 1 = time */
    uint32_t warm;      /* metres, seconds */
    uint8_t  work_type; /* 0 = distance, 1 = time */
    uint32_t work;      /* metres, seconds */
    uint8_t  rest_type; /* 0 = distance, 1 = time */
    uint32_t rest;      /* metres, seconds */
    uint8_t  cool_type; /* 0 = distance, 1 = time */
    uint32_t cool;      /* metres, seconds */
    uint8_t  sets;
} INTERVAL_SETUP_RECORD;

typedef struct
{
    uint8_t type;   /* 1 = warmup, 2 = work, 3 = rest, 4 = cooldown, 5 = finished */
} INTERVAL_START_RECORD;

typedef struct
{
    uint8_t  type;              /* 1 = warmup, 2 = work, 3 = rest, 4 = cooldown */
    uint32_t total_time;        /* seconds */
    float    total_distance;    /* metres */
    uint16_t total_calories;
} INTERVAL_FINISH_RECORD;

typedef struct
{
    time_t   timestamp;     /* utc time */
    uint16_t length;
    uint8_t  *data;
} UNKNOWN_RECORD;

typedef struct _TTBIN_RECORD
{
    struct _TTBIN_RECORD *prev;
    struct _TTBIN_RECORD *next;
    uint16_t length;
    uint8_t  tag;
    union
    {
        uint8_t                data[1];
        GPS_RECORD             gps;
        STATUS_RECORD          status;
        TREADMILL_RECORD       treadmill;
        SWIM_RECORD            swim;
        LAP_RECORD             lap;
        HEART_RATE_RECORD      heart_rate;
        RACE_SETUP_RECORD      race_setup;
        RACE_RESULT_RECORD     race_result;
        TRAINING_SETUP_RECORD  training_setup;
        GOAL_PROGRESS_RECORD   goal_progress;
        INTERVAL_SETUP_RECORD  interval_setup;
        INTERVAL_START_RECORD  interval_start;
        INTERVAL_FINISH_RECORD interval_finish;
    };
} TTBIN_RECORD;

typedef struct
{
    unsigned count;
    TTBIN_RECORD **records;
} RECORD_ARRAY;

typedef struct
{
    uint8_t  file_version;
    uint8_t  firmware_version[4];
    uint16_t product_id;
    time_t   timestamp_local;
    time_t   timestamp_utc;
    unsigned utc_offset;

    uint8_t  activity;
    float    total_distance;
    uint32_t duration;          /* seconds */
    uint16_t total_calories;

    TTBIN_RECORD *race_setup;
    TTBIN_RECORD *race_result;
    TTBIN_RECORD *training_setup;
    TTBIN_RECORD *interval_setup;

    RECORD_ARRAY gps_records;
    RECORD_ARRAY status_records;
    RECORD_ARRAY treadmill_records;
    RECORD_ARRAY swim_records;
    RECORD_ARRAY lap_records;
    RECORD_ARRAY heart_rate_records;
    RECORD_ARRAY goal_progress_records;
    RECORD_ARRAY interval_start_records;
    RECORD_ARRAY interval_finish_records;

    TTBIN_RECORD *first;
    TTBIN_RECORD *last;
} TTBIN_FILE;

/*****************************************************************************/

TTBIN_FILE *read_ttbin_file(FILE *file);

TTBIN_FILE *parse_ttbin_data(uint8_t *data, uint32_t size);

int write_ttbin_file(const TTBIN_FILE *ttbin, FILE *file);

TTBIN_RECORD *insert_before(TTBIN_FILE *ttbin, TTBIN_RECORD *record);

TTBIN_RECORD *insert_after(TTBIN_FILE *ttbin, TTBIN_RECORD *record);

void delete_record(TTBIN_FILE *ttbin, TTBIN_RECORD *record);

const char *create_filename(TTBIN_FILE *file, const char *ext);

void download_elevation_data(TTBIN_FILE *ttbin);

void export_csv(TTBIN_FILE *ttbin, FILE *file);

void export_gpx(TTBIN_FILE *ttbin, FILE *file);

void export_kml(TTBIN_FILE *ttbin, FILE *file);

void export_tcx(TTBIN_FILE *ttbin, FILE *file);

uint32_t export_formats(TTBIN_FILE *ttbin, uint32_t formats);

void free_ttbin(TTBIN_FILE *ttbin);

void replace_lap_list(TTBIN_FILE *ttbin, float *distances, unsigned count);

int truncate_laps(TTBIN_FILE *ttbin);

int truncate_race(TTBIN_FILE *ttbin);

int truncate_goal(TTBIN_FILE *ttbin);

int truncate_intervals(TTBIN_FILE *ttbin);

/*****************************************************************************/

#define OFFLINE_FORMAT_CSV  (0x00000001)
#define OFFLINE_FORMAT_FIT  (0x00000002)
#define OFFLINE_FORMAT_GPX  (0x00000004)
#define OFFLINE_FORMAT_KML  (0x00000008)
#define OFFLINE_FORMAT_PWX  (0x00000010)
#define OFFLINE_FORMAT_TCX  (0x00000020)

typedef struct
{
    uint32_t mask;
    const char *name;
    int requires_gps;
    void (*producer)(TTBIN_FILE* ttbin, FILE *file);
} OFFLINE_FORMAT;

#define OFFLINE_FORMAT_COUNT    (6)
extern const OFFLINE_FORMAT OFFLINE_FORMATS[OFFLINE_FORMAT_COUNT];

#endif  /* __TTBIN_H__ */

