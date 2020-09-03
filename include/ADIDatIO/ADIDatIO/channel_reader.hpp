//#ifndef ADIDATIO_CHANNEL_READER
//#define ADIDATIO_CHANNEL_READER
#pragma once

#include <ADIDatIO/file_reader.hpp>
#include <utils/progress_bar.hpp>

namespace ADIDatIO
{

class ChannelReader
{
public:
    struct Position
    {
        std::size_t record_id;
        std::size_t sample_offset;

        inline
        void nextRecord();
        [[nodiscard]] inline
        Time toTime(const ChannelReader& channel_reader) const;
    };
public:
    inline
    explicit ChannelReader(std::shared_ptr<FileReader> file_reader, std::size_t channel_id);

    void setProgressBar(kvk1920::utils::IProgressBar* progress_bar = nullptr);

public:
    void loadSlice(std::vector<float>& buff,
                   std::size_t record_id,
                   std::size_t start_pos = 0,
                   std::size_t finish_pos = std::numeric_limits<std::size_t>::max());

    void reset();
    [[nodiscard]] inline
    const Position& currentPosition() const;
    inline
    void setPosition(Position pos);

    [[nodiscard]]
    const ChannelInfo& channelInfo() const;

    bool load(std::vector<float>& buff, std::size_t number_of_samples);

    [[nodiscard]]
    const RecordInfo& currentRecord() const;

    [[nodiscard]]
    std::size_t samplesOfThisRecord() const;

    void refreshProgressBar() const;
private:
    [[nodiscard]] int64_t calculateProcessedSamples(Position pos) const;

private:
    std::shared_ptr<FileReader> file_reader_;
    std::size_t channel_id_;
    Position current_pos_;
    kvk1920::utils::IProgressBar* progress_bar_{nullptr};
};

}

namespace ADIDatIO
{

ChannelReader::ChannelReader(std::shared_ptr<FileReader> file_reader, std::size_t channel_id)
        : file_reader_{std::move(file_reader)}
        , channel_id_{channel_id}
        , current_pos_{0, 0}
{
    if (file_reader_->channelsInfo().size() <= channel_id)
        throw std::logic_error("invalid channel id: " + std::to_string(channel_id)
        + ", but FileReader has only " + std::to_string(file_reader_->channelsInfo().size()) + " channels");
}

const ChannelReader::Position&
ChannelReader::currentPosition() const
{
    return current_pos_;
}

void
ChannelReader::setPosition(Position pos)
{
    current_pos_ = pos;
    refreshProgressBar();
}

Time
ChannelReader::Position::toTime(const ChannelReader& channel_reader) const
{
    if (record_id >= channel_reader.channelInfo().records.size())
        return {-1, -1};
    const auto& record{channel_reader.channelInfo().records.at(record_id)};
    Time result{record.start.toTime()};
    result.frac_seconds += record.sample_period * sample_offset;
    result.normalize();
    return result;
}

void
ChannelReader::Position::nextRecord()
{
    ++record_id;
    sample_offset = 0;
}

}
//#endif