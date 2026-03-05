#ifndef GITHUB_API_H
#define GITHUB_API_H

#include <Arduino.h>
#include <HttpClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <vector>

struct ContributionDay {
    String date;
    int count;
};

class GitHubAPI {
public:
    GitHubAPI();
    bool fetchContributions(const String& username, int year, int month, std::vector<ContributionDay>& contributions);
    void setAuthToken(const String& token);

private:
    String authToken;
    WiFiClientSecure client;
    bool parseContributionResponse(const String& json, std::vector<ContributionDay>& contributions, int year, int month);
    String getMonthName(int month);
};

#endif