#include <ADIDatIO/channel_reader.hpp>

namespace ADIDatIO
{

void
ChannelReader::loadSlice(
        std::vector<float>& buff,
        std::size_t record_id,
        std::size_t start_pos,
        std::size_t finish_pos)
{
    return file_reader_->loadSlice(buff, channel_id_, record_id, start_pos, finish_pos);
}

const ChannelInfo&
ChannelReader::channelInfo() const
{
    return file_reader_->channelsInfo().at(channel_id_);
}

std::size_t
ChannelReader::samplesOfThisRecord() const
{
    return currentRecord().number_of_samples - current_pos_.sample_offset;
}

bool
ChannelReader::load(std::vector<float>& buff, std::size_t number_of_samples)
{
    if (file_reader_->numberOfRecords() == current_pos_.record_id)
        return false;
    bool end_of_record{false};
    if (auto residual_samples{samplesOfThisRecord()}; residual_samples <= number_of_samples)
    {
        number_of_samples = residual_samples;
        end_of_record = true;
    }
    // FIXME: remove buff.resize(...)
    buff.resize(number_of_samples);
    loadSlice(buff, current_pos_.record_id, current_pos_.sample_offset,
              current_pos_.sample_offset + number_of_samples);
    current_pos_.sample_offset += number_of_samples;
    if (end_of_record)
        current_pos_.nextRecord();
    refreshProgressBar();
    return true;
}

void
ChannelReader::reset()
{
    current_pos_ = {0, 0};
}

const RecordInfo&
ChannelReader::currentRecord() const
{
    return channelInfo().records.at(current_pos_.record_id);
}

int64_t
ChannelReader::calculateProcessedSamples(Position pos) const
{
    int64_t result{pos.sample_offset};
    for (std::size_t record_id{0}; record_id < pos.record_id; ++record_id)
        result += channelInfo().records[record_id].number_of_samples;
    return result;
}

void
ChannelReader::refreshProgressBar() const
{
    if (progress_bar_)
    {
        progress_bar_->setProgress(calculateProcessedSamples(current_pos_));
        progress_bar_->show();
    }
}

void
ChannelReader::setProgressBar(kvk1920::utils::IProgressBar* progress_bar)
{
    progress_bar_ = progress_bar;
    if (progress_bar)
        progress_bar_->setMaxProgress(calculateProcessedSamples({
            channelInfo().records.size(), 0
        }), true);
}

}