#pragma once

#include <filesystem>

namespace Son32
{

namespace fs = std::filesystem;

struct ChannelConfig
{
    double sample_period;
    float min_value;
    float max_value;
    std::wstring name;
    std::wstring units;

    void calculateScaleInfo(const std::function<bool(std::vector<float>&)>& get_data);
};

struct Time
{
    std::time_t seconds;
    double frac_seconds;
};


struct Config
{
    fs::path path{};
    std::vector<ChannelConfig> channels{};
    Time start;
};

class FileWriter
{
public:

    struct OffsetScale
    {
        float offset;
        float scale;
    };

private:
    explicit FileWriter(const Config& config);

public:
    ~FileWriter();

    [[nodiscard]]
    std::size_t numberOfChannels() const;
    [[nodiscard]]
    Time fileStart() const;
    void write(short channel_id, const std::vector<float>& data);
    void skipTo(Time next_time);

    [[nodiscard]]
    std::size_t expectedSize() const;

    static std::unique_ptr<FileWriter> create(const Config& config);

    std::string path() const { return path_; }

protected:
    static OffsetScale calculateOffsetScale(float min_value, float max_value);

private:
    short handle_;
    std::vector<std::size_t> write_pos_;
    std::vector<OffsetScale> offset_scale_;
    long us_per_time_;
    Time start_;
    std::string path_;
    std::vector<int16_t> conversion_buffer_;
};


}