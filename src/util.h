#include <string>
#include <fstream>

// Classic helper function
class Util
{

public:
    static std::string convertToTime(long int input_seconds);
    static std::string getProgressBar(std::string percent);
    static void getStream(std::string path, std::ifstream &stream);
    static inline std::string &ltrim(std::string &s) {
        s.erase(
            s.begin(), 
            std::find_if(s.begin(), s.end(), 
            [](int c) -> bool {return !std::isspace(c);}));
        return s;
    }
    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            [](int c) -> bool {return !std::isspace(c);}).base(), s.end());
        return s;
    }
};

std::string Util::convertToTime(long int input_seconds)
{
    long minutes = input_seconds / 60;
    long hours = minutes / 60;
    long seconds = int(input_seconds % 60);
    minutes = int(minutes % 60);
    std::string result = std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
    return result;
}
// constructing string for given percentage
// 50 bars is uniformly streched 0 - 100 %
// meaning: every 2% is one bar(|)
std::string Util::getProgressBar(std::string percent)
{

    std::string result = "0%% ";
    int _size = 50;
    int boundaries;
    try
    {
        boundaries = (stof(percent) / 100) * _size;
    }
    catch (...)
    {
        boundaries = 0;
    }

    for (int i = 0; i < _size; i++)
    {
        if (i <= boundaries)
        {
            result += "|";
        }
        else
        {
            result += " ";
        }
    }

    result += " " + percent.substr(0, 5) + " /100%%";
    return result;
}

// wrapper for creating streams
void Util::getStream(std::string path, std::ifstream &stream)
{
    stream.open(path, std::ifstream::in);
    if (!stream && !stream.is_open())
    {
        stream.close();
        throw std::runtime_error("Path is not exist");
    }
    //return stream;
}