#pragma once

#include <utils/ui/progress_bar.hpp>

#include <ostream>
#include <functional>

namespace utils::ui
{

class ConsoleProgressBar : public IProgressBar
{
public:
    explicit ConsoleProgressBar(std::wostream* wout, std::function<std::wstring(int64_t, int64_t)> create_msg);
    explicit ConsoleProgressBar(std::wostream& wout, std::function<std::wstring(int64_t, int64_t)> create_msg);

    explicit ConsoleProgressBar(std::wostream* wout);
    explicit ConsoleProgressBar(std::wostream& wout);

    ~ConsoleProgressBar() override;


    [[nodiscard]] int64_t maxProgress() const noexcept override;

    [[nodiscard]] int64_t progress() const noexcept override;

    void setMaxProgress(int64_t value, bool init) noexcept override;

    void reportJob(int64_t add) override;

    void setProgress(int64_t value) override;

    void show() override;

protected:
    std::wostream* wout_;
    std::function<std::wstring(int64_t, int64_t)> create_msg_;
    int64_t progress_;
    int64_t max_progress_;
    int last_shown_;
};

}