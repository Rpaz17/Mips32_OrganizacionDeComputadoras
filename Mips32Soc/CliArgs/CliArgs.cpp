#include <iostream>
#include <format>
#include <cstring>
#include "CliArgs.hpp"

//--program <ruta del archivo binario del programa en lenguaje de maquina?>
//--font <ruta del archivo MIF con el font>
//--data (Opcional) <ruta de un archivo binario para inicializar el data memory>

void CliArgs::printUsage()
{
    std::cout
        << "Usage: " << program << " --file <csv file path>"
        << "\n"
        << "Options:\n"
        << "  --file <csv file path>          (Required) Specify the CSV file to work with.\n"
        << "  --program <path to binary file>     (Required) Specify the binary file of the program in machine language.\n"
        << "  --font <path to MIF file>           (Required) Specify the MIF file with the font.\n"
        << "  --data <path to binary file>        (Optional) Specify a binary file to initialize the data memory.\n"
        << "\n";
}

std::optional<std::string> CliArgs::parse()
{
    for (int i = 0; i < argc_; i++)
    {

        if (strcmp(argv_[i], "--file") == 0)
        {
            if (i + 1 == argc_)
            {
                return std::string("Option '--file' requires a file path");
            }
            filepath_ = argv_[i + 1];
            i++;
        }
        else if (strcmp(argv_[i], "--program") == 0)
        {
            if (i + 1 == argc_)
            {
                return std::string("Option '--program' requires a file path");
            }
            machineCode_ = argv_[i + 1];
            i++;
        }
        else if (strcmp(argv_[i], "--font") == 0)
        {
            if (i + 1 == argc_)
            {
                return std::string("Option '--font' requires a file path");
            }
            fontpath_ = argv_[i + 1];
            i++;
        }
        else if (strcmp(argv_[i], "--data") == 0)
        {
            if (i + 1 == argc_)
            {
                return std::string("Option '--data' requires a file path");
            }
            dataFilePath_ = argv_[i + 1];
            i++;
        }
        else
        {
            return std::format("Unknown option '{}'", argv_[i]);
        }
    }

    if (filepath_.empty())
    {
        return std::string("Option '--file' is required");
    }
    else if (machineCode_.empty())
    {
        return std::string("Option '--program' is required");
    }
    else if (fontpath_.empty())
    {
        return std::string("Option '--font' is required");
    }

    return std::nullopt;
}
