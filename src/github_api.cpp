#include "../include/github/github_api.h"
#include <ArduinoJson.h>

GitHubAPI::GitHubAPI() {
    client.setInsecure(); // For development - in production, use proper certificate validation
}

void GitHubAPI::setAuthToken(const String& token) {
    authToken = token;
}

bool GitHubAPI::fetchContributions(const String& username, int year, int month, std::vector<ContributionDay>& contributions) {
    Serial.println("GitHubAPI: Starting contribution fetch for " + username + " " + String(year) + "-" + String(month));

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("GitHubAPI: WiFi not connected");
        return false;
    }

    HttpClient http(client, "api.github.com", 443);

    // Set headers
    http.beginRequest();
    http.post("/graphql");
    http.sendHeader("Content-Type", "application/json");
    http.sendHeader("User-Agent", "OJO-GitHub-Display/1.0");
    if (authToken.length() > 0) {
        http.sendHeader("Authorization", "Bearer " + authToken);
    }

    // Calculate date range for the month
    String fromDate = String(year) + "-" + (month < 10 ? "0" : "") + String(month) + "-01";
    String toDate = String(year) + "-" + (month < 10 ? "0" : "") + String(month) + "-31";

    // GraphQL query for contributions
    String query = "{"
        "\"query\": \"query($userName:String!, $from:String!, $to:String!) { "
        "user(login: $userName) { "
        "contributionsCollection(from: $from, to: $to) { "
        "contributionCalendar { "
        "weeks { "
        "contributionDays { "
        "date "
        "contributionCount "
        "} "
        "} "
        "} "
        "} "
        "} "
        "}\", "
        "\"variables\": {"
        "\"userName\": \"" + username + "\", "
        "\"from\": \"" + fromDate + "T00:00:00Z\", "
        "\"to\": \"" + toDate + "T23:59:59Z\""
        "}"
        "}";

    Serial.println("GitHubAPI: Sending GraphQL request...");
    http.println(query);
    http.endRequest();

    int statusCode = http.responseStatusCode();
    String response = http.responseBody();

    Serial.println("GitHubAPI: Response status: " + String(statusCode));
    Serial.println("GitHubAPI: Response length: " + String(response.length()));

    if (statusCode == 200) {
        bool success = parseContributionResponse(response, contributions, year, month);
        if (success) {
            Serial.println("GitHubAPI: Successfully parsed " + String(contributions.size()) + " contribution days");
        } else {
            Serial.println("GitHubAPI: Failed to parse response");
        }
        return success;
    } else {
        Serial.println("GitHubAPI: HTTP request failed, status: " + String(statusCode));
        return false;
    }
}

bool GitHubAPI::parseContributionResponse(const String& json, std::vector<ContributionDay>& contributions, int year, int month) {
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.println("GitHubAPI: JSON deserialization failed: " + String(error.c_str()));
        return false;
    }

    // Check for GraphQL errors
    if (doc["errors"].is<JsonArray>()) {
        Serial.println("GitHubAPI: GraphQL errors in response:");
        for (JsonObject error : doc["errors"].as<JsonArray>()) {
            Serial.println("  " + error["message"].as<String>());
        }
        return false;
    }

    // Navigate to contribution data
    JsonObject data = doc["data"];
    if (!data["user"].is<JsonObject>()) {
        Serial.println("GitHubAPI: No user data in response");
        return false;
    }

    JsonObject user = data["user"];
    if (!user["contributionsCollection"].is<JsonObject>()) {
        Serial.println("GitHubAPI: No contributionsCollection in response");
        return false;
    }

    JsonObject collection = user["contributionsCollection"];
    if (!collection["contributionCalendar"].is<JsonObject>()) {
        Serial.println("GitHubAPI: No contributionCalendar in response");
        return false;
    }

    JsonObject calendar = collection["contributionCalendar"];
    if (!calendar["weeks"].is<JsonArray>()) {
        Serial.println("GitHubAPI: No weeks in contributionCalendar");
        return false;
    }

    // Parse contribution days
    contributions.clear();
    for (JsonObject week : calendar["weeks"].as<JsonArray>()) {
        for (JsonObject day : week["contributionDays"].as<JsonArray>()) {
            ContributionDay contribDay;
            contribDay.date = day["date"].as<String>();
            contribDay.count = day["contributionCount"].as<int>();

            // Only include days from the requested month
            if (contribDay.date.startsWith(String(year) + "-" + (month < 10 ? "0" : "") + String(month))) {
                contributions.push_back(contribDay);
            }
        }
    }

    Serial.println("GitHubAPI: Parsed " + String(contributions.size()) + " contribution days for " + getMonthName(month) + " " + String(year));
    return true;
}

String GitHubAPI::getMonthName(int month) {
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    return months[month - 1];
}