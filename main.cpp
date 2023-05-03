#include <iostream>
#include <curl/curl.h>
#include <string>
#include "json.hpp"

using json = nlohmann::json;    // jsonの記述簡単化

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer)  
{
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
    << "uid: " << "" << "\n"
    << "ISBN: "      << j[0]["summary"]["isbn"].get<std::string>() << "\n"
    << "title: "     << j[0]["summary"]["title"].get<std::string>() << "\n"
    // << j[0]["summary"]["series"] << "\n"
    // << j[0]["summary"]["volume"] << "\n"
    << "author: "    << j[0]["summary"]["author"].get<std::string>() << "\n"
    << "publisher: " << j[0]["summary"]["publisher"].get<std::string>() << "\n"
    << "pubdate: "   << j[0]["summary"]["pubdate"].get<std::string>() << "\n"
    << "url: "       << j[0]["summary"]["cover"].get<std::string>() << "\n"
    << "---" << "\n";
  curl_easy_cleanup(curl);
  return EXIT_SUCCESS;
}
