#pragma once

#include <ADIDatIO/common.hpp>
#include <filesystem>

namespace ADIDatIO
{

namespace fs = std::filesystem;

class FileReader : public std::enable_shared_from_this<FileReader>
{
protected:
    class Impl;
    struct MakeSharedEnabler;
protected:
    explicit FileReader(const fs::path& path);

public:
    ~FileReader() = default;

public:
    static std::shared_ptr<FileReader> load(const fs::path& path);

public:
    [[nodiscard]]
    std::size_t numberOfRecords() const;

    [[nodiscard]]
    const std::vector<ChannelInfo>& channelsInfo() const;

    void loadSlice(std::vector<float>& buff,
                    std::size_t channel_id,
                    std::size_t record_id,
                    std::size_t start_pos = 0,
                    std::size_t finish_pos = std::numeric_limits<std::size_t>::max()) const;

    [[nodiscard]]
    Time fileStart() const;

private:
    std::unique_ptr<Impl> pimpl_;
};

}
