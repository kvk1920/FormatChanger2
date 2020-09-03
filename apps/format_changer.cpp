#include <time.h>
#include <ADIDatCAPI_mex.h>

#include <windows.h>
#include <Son.h>

extern "C" {
#include <tinyfiledialogs.h>
}

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>


#define FMCH_MAX_CHANNELS 32

// #define TEST_CHANNEL 0

// #define TEST_RECORD 0

#ifdef TEST_CHANNEL
#define CHANNEL_LOOP(channel, num_channels) for (long channel = TEST_CHANNEL; channel <= TEST_CHANNEL; channel++)
#else
#define CHANNEL_LOOP(channel, num_channels) for (long channel = 0; channel < num_channels; channel++)
#endif

#ifdef TEST_RECORD
#define FIRST_RECORD TEST_RECORD
#define RECORD_LOOP(record, num_records) for (long record = TEST_RECORD; record <= TEST_RECORD; record++)
#else
#define FIRST_RECORD 0
#define RECORD_LOOP(record, num_records) for (long record = 0; record < num_records; record++)
#endif


static void
print_ADI_error_message(ADIResultCode err)
{
    static const char* white = "\033[0;37m";
    static const char* red   = "\033[0;31m";
    if (err != kResultSuccess)
        printf("%s", red);
    static wchar_t buff[1024];
    long len;
    (void)ADI_GetErrorMessage(err, buff, 1024, &len);
    wchar_t* actual_buff = buff;
    if (len > 1024)
    {
        actual_buff = (wchar_t*)malloc(len * sizeof(wchar_t));
        (void)ADI_GetErrorMessage(err, actual_buff, len, NULL);
    }
    printf("%S\n", actual_buff);
    if (buff != actual_buff)
        free(actual_buff);
    if (err != kResultSuccess)
        printf("%s", white);
}

static void
print_SON_error_message(const long err)
{
    if (err >= 0)
    {
        puts("SON_SUCCESS");
        return;
    }
    static const char* white = "\033[0;37m";
    static const char* red   = "\033[0;31m";
    printf("%s", red);
    const char* msg = "Unknown error";
    if (-1 == err)  msg = "SON_NO_FILE";
    if (-4 == err)  msg = "SON_NO_HANDLES";
    if (-5 == err)  msg = "SON_NO_ACCESS";
    if (-6 == err)  msg = "SON_BAD_HANDLE";
    if (-8 == err)  msg = "SON_OUT_OF_MEMORY";
    if (-17 == err) msg = "SON_BAD_READ";
    if (-18 == err) msg = "SON_BAD_WRITE";
    if (-9 == err)  msg = "SON_NO_CHANNEL";
    if (-10 == err) msg = "SON_CHANNEL_USED";
    if (-11 == err) msg = "SON_CHANNEL_UNUSED";
    if (-12 == err) msg = "SON_PAST_EOF";
    if (-13 == err) msg = "SON_WRONG_FILE";
    if (-14 == err) msg = "SON_NO_EXTRA";
    if (-19 == err) msg = "SON_CORRUPT_FILE";
    if (-20 == err) msg = "SON_PAST_SOF";
    if (-21 == err) msg = "SON_READ_ONLY";
    if (-22 == err) msg = "SON_BAD_PARAM";
    puts(msg);
    printf("%s", white);
}

inline static long
gcd(long a, long b)
{
    static long c;
    while (b)
    {
        a %= b;
        c = b;
        b = a;
        a = c;
    }
    return a;
}

inline static void
set_error(bool* error)
{
    if (NULL != error)
        *error = true;
}

inline static void
get_channel_sample_periods(ADI_FileHandle input,
                           long number_of_channels,
                           double* channel_sample_periods,
                           bool* error)
{
    puts("trying to get channel sample periods...");
    // for (long channel = 0; channel < number_of_channels; channel++)
    CHANNEL_LOOP(channel, number_of_channels)
    {
        printf("trying to get sample period for channel %li...\n", channel);
        ADIResultCode code = ADI_GetRecordSamplePeriod(input,
                                                       channel,
                                                       0,
                                                       channel_sample_periods + channel);
        print_ADI_error_message(code);
        if (kResultSuccess != code)
        {
            set_error(error);
            return;
        }
    }
}

inline static long
determine_optimal_us_per_time(long number_of_channels,
                              const double* channel_sample_periods)
{
    // следующая строчка не нужна, т.к. ошибок в функции не бывает
    // puts("trying to determine optimal ticks per second...");
    long base = 1000000;
    //for (long channel = 0; channel < number_of_channels; channel++)
    CHANNEL_LOOP(channel, number_of_channels)
        base = gcd(base, (long)round(channel_sample_periods[channel] * 1000000));
    printf("optimal sample period for output file is %li (microseconds)\n", base);
    return base;
}

struct io_files
{
    const wchar_t* input_path;
    const char* output_path;
};
typedef struct io_files files_t;

static files_t
get_file_names(const wchar_t* file_name_begin,
               const wchar_t* file_name_end)
{
    static wchar_t input_path[MAX_PATH * 2];
    static char output_path[MAX_PATH * 2];
    int len = file_name_end - file_name_begin;
    wmemcpy(input_path, file_name_begin, len);
    input_path[len] = L'\0';
    for (int i = 0; i < len - 6; ++i)
        output_path[i] = (char)input_path[i];
    memcpy(output_path + len - 6, "SMR", 4);
    files_t result = {input_path, output_path};
    return result;
}

inline static ADI_FileHandle
open_input_file(const wchar_t* path, bool* error)
{
    puts("opening input file...");
    ADI_FileHandle handler;
    ADIResultCode code = ADI_OpenFile(path, &handler, kOpenFileForReadOnly);
    print_ADI_error_message(code);
    if (code != kResultSuccess)
        set_error(error);
    return handler;
}


typedef char* channel_name_t;

struct channels_info
{
    long number_of_channels;
    double* sample_periods;
    long optimal_us_per_time;
    channel_name_t* names;
    channel_name_t* units;
    float* scales;
};
typedef struct channels_info channels_info_t;

inline static void
destroy_channels_info(channels_info_t* info)
{
    if (NULL != info->sample_periods)
    {
        free(info->sample_periods);
        info->sample_periods = NULL;
    }
    if (NULL != info->names)
    {
        for (int i = 0; i < info->number_of_channels; i++)
            if (NULL != info->names[i])
                free(info->names[i]);
        free(info->names);
        info->names = NULL;
    }
    if (NULL != info->units)
    {
        for (int i = 0; i < info->number_of_channels; i++)
            if (NULL != info->units[i])
                free(info->units[i]);
        free(info->units);
        info->units = NULL;
    }
    if (NULL != info->scales)
    {
        free(info->scales);
        info->scales = NULL;
    }
}

static bool
get_channel_name(ADI_FileHandle input,
                 long channel,
                 channels_info_t* info)
{
    printf("trying to get name for channel %li...\n", channel);

    static wchar_t name[256];
    long len;
    ADIResultCode code = ADI_GetChannelName(input, channel, name, 255, &len);
    print_ADI_error_message(code);
    if (kResultSuccess != code)
        return true;
    printf("channel name is:\n%S\n", name);
    if (len > SON_TITLESZ)
        len = SON_TITLESZ;
    info->names[channel] = (char*)malloc(len + 1);
    for (int i = 0; i < len; i++)
        info->names[channel][i] = (char)name[i];
    info->names[channel][len] = '\0';
    return false;
}

static bool
get_channel_units(ADI_FileHandle input,
                  long channel,
                  channels_info_t* info)
{
    static wchar_t name[256];
    printf("trying to get units for channel %li...\n", channel);
    long len;
    ADIResultCode code = ADI_GetUnitsName(input, channel, 0, name, 256, &len);
    print_ADI_error_message(code);
    if (kResultSuccess != code)
        return true;
    printf("channel units is:\n%S\n", name);

    if (len > SON_UNITSZ)
        len = SON_UNITSZ;

    info->units[channel] = (char*)malloc(len + 1);
    for (int i = 0; i < len; i++)
        info->units[channel][i] = (char)name[i];
    info->units[channel][len] = '\0';

    return false;
}

inline static bool
calculate_scale(ADI_FileHandle input, long channel, long number_of_records, float* scale)
{
    printf("trying to calculate optimal scale for channel %li...\n", channel);
    float max = .0;
    float* buffer = NULL;
    bool error = false;
    // for (long record = 0; record < number_of_records; record++)
    RECORD_LOOP(record, number_of_records)
    {
        long len;
        ADIResultCode code = ADI_GetNumSamplesInRecord(input, channel, record, &len);
        if (kResultSuccess != code)
        {
            puts("error in function GetNumSamplesInRecord()");
            print_ADI_error_message(code);
            error = true;
            goto cleanup;
        }
        buffer = (float*)realloc(buffer, len * sizeof(float));
        code = ADI_GetSamples(input, channel, record, 0, kADICDataAtSampleRate, len, buffer, &len);
        if (kResultSuccess != code)
        {
            puts("error in function GetSamples()");
            print_ADI_error_message(code);
            error = true;
            goto cleanup;
        }
        for (int i = 0; i < len; i++)
            if (fabs(buffer[i]) > max)
                max = fabs(buffer[i]);
    }
    if (max < 1.)
        max = 1.;
    *scale = max / 4;
    printf("calculated scale: %f\n", *scale);
  cleanup:
    if (buffer != NULL)
        free(buffer);
    return error;
}

inline static channels_info_t
get_channels_info(ADI_FileHandle input,
                  long number_of_records,
                  bool* error)
{
    channels_info_t result;
    result.number_of_channels = -1;
    result.sample_periods = NULL;
    result.names = result.units = NULL;
    result.scales = NULL;

    puts("trying to get number of channels...");
    ADIResultCode code = ADI_GetNumberOfChannels(input, &result.number_of_channels);
    print_ADI_error_message(code);
    if (kResultSuccess != code)
    {
        set_error(error);
        goto cleanup;
    }

    result.sample_periods = (double*)malloc(result.number_of_channels * sizeof(double));

    result.names = (char**)calloc(result.number_of_channels, sizeof(char*));
    result.units = (char**)calloc(result.number_of_channels, sizeof(char*));
    result.scales = (float*)malloc(sizeof(float) * result.number_of_channels);


    // for (long channel = 0; channel < result.number_of_channels; channel++)
    CHANNEL_LOOP(channel, result.number_of_channels)
    {
        printf("trying to get sample period for channel %li...\n", channel);
        code = ADI_GetRecordSamplePeriod(input, channel, 0, result.sample_periods + channel);
        print_ADI_error_message(code);
        if (kResultSuccess != code)
        {
            if (NULL != error)
                *error = true;
            goto cleanup;
        }
        printf("sample period if %.6f seconds\n", result.sample_periods[channel]);

        if (get_channel_name(input, channel, &result))
        {
            set_error(error);
            goto cleanup;
        }

        if (get_channel_units(input, channel, &result))
        {
            set_error(error);
            goto cleanup;
        }

        if (calculate_scale(input, channel, number_of_records, result.scales + channel))
        {
            set_error(error);
            goto cleanup;
        }
    }

    result.optimal_us_per_time = determine_optimal_us_per_time(result.number_of_channels, result.sample_periods);

    return result;

  cleanup:
    destroy_channels_info(&result);
    return result;
}

struct record_time
{
    long seconds;
    double frac_seconds;
};
typedef struct record_time record_time_t;

inline static int64_t
calc_diff_in_microseconds(record_time_t t1,
                   record_time_t t2) // microseconds
{
    int64_t result = t2.seconds - t1.seconds;
    result *= 1000000;
    result += (long)round((t2.frac_seconds - t1.frac_seconds) * 1000000LL);
    return result;
}

inline static bool
get_record_time(ADI_FileHandle input, long record, record_time_t* t)
{
    printf("trying to get start time of record %li...\n", record);
    static char unused[32];
    ADIResultCode code;
    code = ADI_GetRecordTime(input, record, &t->seconds, &t->frac_seconds, (long*)unused);
    print_ADI_error_message(code);
    return kResultSuccess != code;
}

inline static short
create_output_file(const char* path,
                   const channels_info_t* info,
                   record_time_t file_time,
                   bool* error)
{
    puts("creating output file...");
    short output = SONCreateFile(path, 0, 0);
    print_SON_error_message((long)output);
    if (output < 0)
    {
        set_error(error);
        goto cleanup_on_error;
    }

    {
    SONSetFileClock(output, info->optimal_us_per_time, 1);

    // for (long channel = 0; channel < info->number_of_channels; channel++)
    CHANNEL_LOOP(channel, info->number_of_channels)
    {
        short code = SONSetWaveChan(output, channel, -1,
                                    ((long)round(info->sample_periods[channel] * 1000000) / info->optimal_us_per_time),
                                    4096, "",
                                    info->names[channel],
                                    info->scales[channel], 0,
                                    info->units[channel]
                                    );
        if (code < 0)
        {
            print_SON_error_message(code);
            set_error(error);
            goto cleanup_on_error;
        }
    }
    }
    {
    const struct tm* time_info = gmtime(&file_time.seconds);

    TSONTimeDate output_time;
    output_time.ucHun = (uint8_t)(round(file_time.frac_seconds * 100));
    output_time.ucSec = time_info->tm_sec;
    output_time.ucMin = time_info->tm_min;

    output_time.ucHour = time_info->tm_hour;
    output_time.ucDay = time_info->tm_mday;

    output_time.ucMon = time_info->tm_mon + 1;
    output_time.wYear = time_info->tm_year + 1900;


    SONTimeDate(output, NULL, &output_time);
    }
    return output;
  cleanup_on_error:
    puts("closing output file...");
    print_SON_error_message(SONCloseFile(output));
    return output;
}

inline static bool
get_num_samples(ADI_FileHandle input, long channel, long record, long* number_of_samples)
{
    printf("trying to get number of samples in record %li...\n", record);
    ADIResultCode code = ADI_GetNumSamplesInRecord(input, channel, record, number_of_samples);
    print_ADI_error_message(code);
    return kResultSuccess != code;
}


static bool
transfer_channel(ADI_FileHandle input,
                 short output,
                 const channels_info_t* info,
                 long number_of_records,
                 long channel)
{
    printf("starting to convert channel %li(\"%s\")\n", channel, info->names[channel]);
    float* input_buff = NULL;
    short* output_buff = NULL;
    bool error = false;

    record_time_t start_time;
    if (get_record_time(input, FIRST_RECORD, &start_time))
        return true;

    int flag;
    ADIResultCode code;
    static char unused[32];

    // for (long record = 0; record < number_of_records; record++)
    RECORD_LOOP(record, number_of_records)
    {
        long samples;
        record_time_t current_time;
        if (get_record_time(input, record, &current_time) || get_num_samples(input, channel, record, &samples))
        {
            error = true;
            goto cleanup;
        }
        // TODO: проверить это всё
        long start_tick = (long)(calc_diff_in_microseconds(start_time, current_time) / info->optimal_us_per_time);
        input_buff = (float*)realloc((void*)input_buff, samples * sizeof(float));
        code = ADI_GetSamples(input, channel, record, 0, kADICDataAtSampleRate, samples, input_buff, (long*)unused);
        if (kResultSuccess != code)
        {
            print_ADI_error_message(code);
            error = true;
            goto cleanup;
        }
        output_buff = (short*)realloc(output_buff, samples * sizeof(short));
        for (int i = 0; i < samples; i++)
        {
            short x = (short)round((input_buff[i] / info->scales[channel]) * 6553.6);
            output_buff[i] = x;
        }
        flag = SONWriteADCBlock(output, channel, output_buff, samples, start_tick);
        if (flag < 0)
        {
            print_SON_error_message(flag);
            error = true;
            goto cleanup;
        }
        SONCommitFile(output, FALSE);
    }
  cleanup:
    if (NULL != input_buff)
        free(input_buff);
    if (NULL != output_buff)
        free(output_buff);
    return error;
}


static void
convert_file(const wchar_t* input_path, const char* output_path)
{
    printf("input_file:\n%S\noutput_file:\n%s\n",
           input_path, output_path);
    bool error = false;
    bool success = false;
    ADI_FileHandle input = open_input_file(input_path, &error);
    if (error)
        return;


    long number_of_records;

    puts("trying to get number of recods...");
    ADIResultCode adi_code = ADI_GetNumberOfRecords(input, &number_of_records);
    print_ADI_error_message(adi_code);
    if (kResultSuccess != adi_code)
        goto cleanup3;
    printf("number of records is %li\n", number_of_records);
    {
    channels_info_t info = get_channels_info(input, number_of_records, &error);
    if (error)
        goto cleanup2;

    record_time_t start_time;
    get_record_time(input, FIRST_RECORD, &start_time);


    {

    short output = create_output_file(output_path,
                                      &info,
                                      start_time,
                                      &error);
    if (error)
        goto cleanup;
    short flag;
    flag = SONSetBuffering(output, -1, 4000000);
    if (flag != 0)
    {
        puts("something gone wrong in function SetBuffering...");
        print_SON_error_message(flag);
        goto cleanup;
    }

    flag = SONSetBuffSpace(output);

    if (flag != 0)
    {
        puts("something gone wrong in function SetBuffspace...");
        print_SON_error_message(flag);
        goto cleanup;
    }


    // for (long channel = 0; channel < info.number_of_channels; channel++)
    CHANNEL_LOOP(channel, info.number_of_channels)
        if (transfer_channel(input, output, &info,
                             number_of_records,
                             channel))
            goto cleanup;
    puts("all channels converted");
    success = true;
  cleanup:
    SONCommitFile(output, TRUE);
    puts("closing output file...");
    print_SON_error_message(SONCloseFile(output));
    }
  cleanup2:
    destroy_channels_info(&info);
    }
  cleanup3:
    puts("closing input file...");
    print_ADI_error_message(ADI_CloseFile(&input));
    if (success)
    {
        puts("output file:");
        puts(output_path);
        puts("\n\n\n");
    }
}

int
main()
{
    static const wchar_t* FILTERS[] = {L"*.adicht"};
    // files - список путей, разделённых '|'
    const wchar_t* files = tinyfd_openFileDialogW(
                L"Файлы в формате .adicht",
                L"",
                1,
                /*(const wchar_t*[])*/ //{L"*.adicht"},
                FILTERS,
                NULL,
                1
                );
    if (NULL == files)
    {
        puts("no files choosen");
        return 0;
    }
    const wchar_t* start = files;
    const wchar_t* end = files;
    while (1)
    {
        while (*end != L'|' && *end != L'\0')
            ++end;
        files_t files = get_file_names(start, end);
        convert_file(files.input_path, files.output_path);
        if (L'\0' == *end)
            break;
        start = end + 1;
        end = start;
    }
    puts("press Enter to exit");
    system("PAUSE");
}
