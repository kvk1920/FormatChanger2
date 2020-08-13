#include <string>
#include <vector>

namespace ADIDatIO
{
namespace detail
{
#include <ADIDatCAPI_mex.h>
}

class Reader
{
private:
    Reader();
    void open(const wchar_t* path);
    void close();
public:
    explicit Reader(const wchar_t* path);
    ~Reader();


    size_t get_number_of_records();
    size_t get_number_of_channels();
    double get_sample_period(size_t channel);
    std::string get_channel_name(size_t channel);
    std::string get_units_name(size_t channel);
    std::vector<float> get_record(size_t channel, size_t record);
    double get_channel_sample_period(size_t channel);

    struct Time
    {
        long seconds; // since 1980
        double frac_seconds;
    };

    Time get_record_time(size_t record);

private:
    //void ensure(std::string op);

private:
    detail::ADI_FileHandle* _handle;
};

}
