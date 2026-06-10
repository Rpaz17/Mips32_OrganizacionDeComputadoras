#pragma once

#include <string>
#include <optional>

class CliArgs
{
public:
    CliArgs(int argc, const char *argv[])
        : argc_(argc - 1), argv_(argv + 1), program(argv[0])
    {
    }

    std::string filePath() const
    {
        return filepath_;
    }

    std::string machineCode() const
    {
        return machineCode_;
    }

    std::string fontPath() const
    {
        return fontpath_;
    }

    std::optional<std::string> dataFilePath() const
    {
        return dataFilePath_;
    }

    void printUsage();

    [[nodiscard]] std::optional<std::string> parse();

private:
    int argc_;
    const char **argv_;
    std::string program{};
    std::string filepath_{};
    std::string fontpath_{};
    std::string machineCode_{};
    std::optional<std::string> dataFilePath_{};
};