#include <ADIDatIO/file_reader.hpp>
#include <ADIDatIO/errors.hpp>

namespace ADIDatIO
{

class FileReader::Impl
{
public:
    explicit Impl(const fs::path &path);
    ~Impl();

public:
    [[nodiscard]]
    std::size_t numberOfRecords() const;
    [[nodiscard]]
    Time fileStart() const;
    [[nodiscard]]
    const std::vector<ChannelInfo>& channelsInfo() const;

    void loadSlice(
            std::vector<float>& buff,
            std::size_t channel_id,
            std::size_t record_id,
            std::size_t start_pos,
            std::size_t finish_pos
            ) const;


private:
    ADI_FileHandle handle_;
    std::vector<ChannelInfo> channels_;
    Time file_start_;
    std::size_t number_of_records_;
};

}

// FileReader::Impl
namespace ADIDatIO
{

using namespace detail;

namespace
{

inline const wchar_t *
wcharRead(
        const std::function<ADIResultCode(
                wchar_t *,       // buffer
                long,           // actual buffer size
                long *           // text length
        )>& read_func
)
{
    std::vector<wchar_t> wchar_buff;
    long text_length{0};
    ADIResultCode code{read_func(wchar_buff.data(), wchar_buff.size(), &text_length)};
    if (ADIResultCode::kResultBufferTooSmall == code)
    {
        wchar_buff.resize(text_length + 1);
        code = read_func(wchar_buff.data(), wchar_buff.size(), &text_length);
    }
    checkException(code);
    wchar_buff.resize(text_length);
    wchar_buff.push_back(L'\0');
    return wchar_buff.data();
}

}



FileReader::Impl::Impl(const fs::path &path)
{
    checkException(ADI_OpenFile(path.wstring().c_str(), &handle_, ADIFileOpenMode::kOpenFileForReadOnly));
    long number_of_channels, number_of_records;
    checkException(ADI_GetNumberOfChannels(handle_, &number_of_channels));
    checkException(ADI_GetNumberOfRecords(handle_, &number_of_records));
    number_of_records_ = number_of_records;
    channels_.resize(number_of_channels);
    bool file_start_initialized{false};
    for (long channel_id{0}; channel_id < number_of_channels; ++channel_id)
    {
        auto& channel{channels_.at(channel_id)};
        channel.name = wcharRead([handle = handle_, channel_id](
                wchar_t* buff,
                long actual_buff_size,
                long* text_length
                ) -> ADIResultCode {
            return ADI_GetChannelName(handle, channel_id, buff, actual_buff_size, text_length);
        });
        channel.units = wcharRead([handle = handle_, channel_id](
                wchar_t* buff,
                long actual_buff_size,
                long* text_length
                ) -> ADIResultCode {
            // TODO: check if here different units name in records
            return ADI_GetUnitsName(handle, channel_id, 0L, buff, actual_buff_size, text_length);
        });
        channel.records.resize(number_of_records);
        for (long record_id{0}; record_id < number_of_records; ++record_id)
        {
            auto& info{channel.records.at(record_id)};
            checkException(ADI_GetRecordSamplePeriod(handle_, channel_id, record_id, &info.sample_period));
            long number_of_samples;
            checkException(ADI_GetNumSamplesInRecord(handle_, channel_id, record_id, &number_of_samples));
            info.number_of_samples = number_of_samples;
            checkException(ADI_GetRecordTickPeriod(handle_, channel_id, record_id, &info.start.tick_period));
            checkException(ADI_GetRecordTime(handle_, record_id, &info.start.seconds, &info.start.frac_seconds, &info.start.tick_shift));
            info.start.tick_shift *= -1;
            if (file_start_initialized)
                file_start_ = std::min(file_start_, info.start.toTime());
            else
            {
                file_start_ = info.start.toTime();
                file_start_initialized = true;
            }
        }
    }
}

FileReader::Impl::~Impl()
{
    checkException(ADI_CloseFile(&handle_));
}

std::size_t
FileReader::Impl::numberOfRecords() const
{
    return number_of_records_;
}

const std::vector<ChannelInfo>&
FileReader::Impl::channelsInfo() const
{
    return channels_;
}

Time
FileReader::Impl::fileStart() const
{
    return file_start_;
}

void
FileReader::Impl::loadSlice(std::vector<float> &buff, std::size_t channel_id, std::size_t record_id,
                            std::size_t start_pos, std::size_t finish_pos) const
{
    if (const auto number_of_samples{channels_.at(channel_id).records.at(record_id).number_of_samples};
    finish_pos > number_of_samples
    )
        finish_pos = number_of_samples;
    start_pos = std::min(start_pos, finish_pos);
    buff.resize(finish_pos - start_pos);
    std::size_t done{0};
    long returned{0};
    while (done < buff.size())
    {
        checkException(ADI_GetSamples(handle_, channel_id, record_id, start_pos + done,
                                      ADICDataFlags::kADICDataAtSampleRate,
                                      buff.size() - done, buff.data() + done,
                                      &returned));
        done += returned;
    }
}

}

// FileReader
namespace ADIDatIO
{

FileReader::FileReader(const fs::path &path)
: pimpl_{std::make_unique<Impl>(path)}
{}

std::size_t
FileReader::numberOfRecords() const
{
    return pimpl_->numberOfRecords();
}

const std::vector<ChannelInfo>&
FileReader::channelsInfo() const
{
    return pimpl_->channelsInfo();
}

void
FileReader::loadSlice(std::vector<float> &buff, std::size_t channel_id, std::size_t record_id, std::size_t start_pos,
                      std::size_t finish_pos) const
{
    pimpl_->loadSlice(buff, channel_id, record_id, start_pos, finish_pos);
    updateProgressBar(buff.size());
}

Time
FileReader::fileStart() const
{
    return pimpl_->fileStart();
}

struct FileReader::MakeSharedEnabler : public FileReader {
    explicit MakeSharedEnabler(const fs::path& path) : FileReader(path)
    {}
};


std::shared_ptr<FileReader>
FileReader::load(const fs::path &path)
{
    return std::make_shared<MakeSharedEnabler>(path);
}

void
FileReader::setProgressBar(kvk1920::utils::IProgressBar* pb)
{
    progress_bar_ = pb;
}

void
FileReader::updateProgressBar(int64_t number_of_read_samples) const
{
    if (progress_bar_)
    {
        progress_bar_->reportJob(number_of_read_samples);
        progress_bar_->show();
    }
}

}