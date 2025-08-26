#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    const std::string url =
        "https://raw.githubusercontent.com/anilkruz/Json/refs/heads/main/nidhi.json";
    std::string readBuffer;

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error initializing libcurl\n";
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "curl error: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_easy_cleanup(curl);

    try {
        json j = json::parse(readBuffer);

        for (const auto& floor : j.at("floors")) {
            int floorNo = floor.at("floorNo");
            std::cout << "Floor " << floorNo << " has slots:\n";
            for (const auto& slot : floor.at("slots")) {
                std::cout << "  id: " << slot.at("id")
                          << ", type: " << slot.at("type") << "\n";
            }
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return 1;
    } catch (const json::exception& e) {
        std::cerr << "JSON error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

