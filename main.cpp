#include <iostream>
#include <curl/curl.h>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "json.hpp"

using json = nlohmann::json;    // jsonの記述簡単化

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer) {
    int result = 0;
    if (buffer != NULL) {  
        buffer->append(data, size * nmemb);  
        result = size * nmemb;  
    }  
    return result;  
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage:\n\t ./main <ISBN>\n";
    return EXIT_FAILURE;
  }

  
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm* lt = std::localtime(&t);
  std::stringstream ss;
  ss << lt->tm_year + 1900 
    << std::setfill('0') << std::right << std::setw(2) << lt->tm_mon + 1 
    << std::setfill('0') << std::right << std::setw(2) << lt->tm_mday 
    << std::setfill('0') << std::right << std::setw(2) << lt->tm_hour 
    << std::setfill('0') << std::right << std::setw(2) << lt->tm_min 
    << std::setfill('0') << std::right << std::setw(2) << lt->tm_sec;

  std::string isbn = argv[1];

  std::string buf;
  CURL *curl;

  curl = curl_easy_init();
  std::string base_url = "https://api.openbd.jp/v1/get?isbn=";
  std::string url = base_url + isbn; //"9784004313724";
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
  curl_easy_perform(curl);

  json j = json::parse(buf.c_str());
  std::cout << "---" << "\n"
    << "uid: " << ss.str() << "\n"
    << "ISBN: "      << j[0]["summary"]["isbn"].get<std::string>() << "\n"
    << "title: "     << j[0]["summary"]["title"].get<std::string>() << "\n"
    // << j[0]["summary"]["series"] << "\n"
    // << j[0]["summary"]["volume"] << "\n"
    << "author: "    << j[0]["summary"]["author"].get<std::string>() << "\n"
    << "publisher: " << j[0]["summary"]["publisher"].get<std::string>() << "\n"
    << "pubdate: "   << j[0]["summary"]["pubdate"].get<std::string>() << "\n"
    << "cover: "       << j[0]["summary"]["cover"].get<std::string>() << "\n"
    << "---" << "\n";

  std::cout << "---" << "\n"
    << j[0]["onix"]["CollateralDetail"]["TextContent"][1]["Text"].get<std::string>() << "\n";
  curl_easy_cleanup(curl);

  std::cout << "\n" << url << "&pretty\n";
  return EXIT_SUCCESS;
}
