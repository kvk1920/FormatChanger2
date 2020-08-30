#pragma once

#include <memory>

namespace kvk1920::utils
{

class IProgressBar
{
public:
    [[nodiscard]]
    virtual int64_t maxProgress() const noexcept = 0;
    [[nodiscard]]
    virtual int64_t progress() const noexcept = 0;
    virtual void setMaxProgress(int64_t value, bool init = true) noexcept = 0;
    virtual void reportJob(int64_t add = 1) = 0;
    virtual void setProgress(int64_t value = 0) = 0;
    virtual ~IProgressBar() = default;
    virtual void show();
};

}