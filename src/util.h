#include <string>
#include <fstream>
#include <algorithm>
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