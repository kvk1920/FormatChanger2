#include <utils/console_progress_bar.hpp>

#include <sstream>

namespace kvk1920::utils
{

ConsoleProgressBar::ConsoleProgressBar(std::wostream* wout, std::function<std::wstring(int64_t, int64_t)> create_msg)
: wout_{wout}
, create_msg_{std::move(create_msg)}
{
    progress_ = 0;
    max_progress_ = 0;
    last_shown_ = -1;
    *wout_ << std::endl;
}

ConsoleProgressBar::ConsoleProgressBar(std::wostream& wout, std::function<std::wstring(int64_t, int64_t)> create_msg)
: ConsoleProgressBar(&wout, std::move(create_msg))
{}

const std::size_t LINE_LENGTH = 50;

std::wstring
defaultMsg(int64_t progress, int64_t max_progress)
{
    std::wostringstream string_builder;
    const auto default_width{string_builder.width()};
    string_builder.width(3);
    string_builder << progress * 100 / max_progress;
    string_builder.width(default_width);
    string_builder << L"% ";
    string_builder << std::wstring(LINE_LENGTH * progress / max_progress, L'+');
    string_builder << std::wstring(LINE_LENGTH - LINE_LENGTH * progress / max_progress, L'-');
    return string_builder.str();
}

ConsoleProgressBar::ConsoleProgressBar(std::wostream* wout)
: wout_{wout}
, create_msg_{defaultMsg}
{
    progress_ = max_progress_ = 0;
    last_shown_ = -1;
}

ConsoleProgressBar::ConsoleProgressBar(std::wostream& wout)
: ConsoleProgressBar(&wout)
{}

ConsoleProgressBar::~ConsoleProgressBar()
{
    *wout_ << std::endl;
}

void
ConsoleProgressBar::setMaxProgress(int64_t value, bool init) noexcept
{
    max_progress_ = value;
    if (init)
    {
        progress_ = 0;
        last_shown_ = -1;
        show();
    }
}

void
ConsoleProgressBar::reportJob(int64_t add)
{
    progress_ += add;
    if (progress_ > max_progress_)
        progress_ = max_progress_;
    show();
}

void
ConsoleProgressBar::show()
{
    int64_t to_show{progress_ * 100 / max_progress_};
    if (to_show != last_shown_)
    {
        *wout_ << L'\r' << create_msg_(progress_, max_progress_);
        last_shown_ = to_show;
    }
}

int64_t
ConsoleProgressBar::progress() const noexcept
{
    return progress_;
}

int64_t
ConsoleProgressBar::maxProgress() const noexcept
{
    return max_progress_;
}

void
ConsoleProgressBar::setProgress(int64_t value)
{
    progress_ = value;
}

}