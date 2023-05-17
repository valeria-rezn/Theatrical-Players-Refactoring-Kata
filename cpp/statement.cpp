#include "statement.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// https://stackoverflow.com/a/7277333/104370
class comma_numpunct : public std::numpunct<char>
{
  protected:
    virtual char do_thousands_sep() const override
    {
        return ',';
    }

    virtual std::string do_grouping() const override
    {
        return "\03";
    }
};

enum class PlayType {
    TRAGEDY,
    COMEDY,
    HISTORY,
    PASTORAL,
    UNKNOWN
};

PlayType getPlayType(const nlohmann::json& playType) {
    if (playType == "tragedy") {
        return PlayType::TRAGEDY;
    }
    else if (playType == "comedy") {
        return PlayType::COMEDY;
    }
    else if (playType == "history") {
        return PlayType::HISTORY;
    }
    else if (playType == "pastoral") {
        return PlayType::PASTORAL;
    }
    else
    {
        return PlayType::UNKNOWN;
    }
}

float Calculate_TragedyAmount(const nlohmann::json& performance,float &this_amount)
{
    this_amount = 40000;
    if (performance["audience"] > 30)
    {
        this_amount += 1000 * (performance["audience"].get<int>() - 30);
    }
    return this_amount;
}

float Calculate_ComedyAmount(const nlohmann::json& performance, float& this_amount)
{
    this_amount = 30000;
    if (performance["audience"] > 20)
    {
        this_amount += 10000 + 500 * (performance["audience"].get<int>() - 20);
    }
    this_amount += 300 * performance["audience"].get<int>();
    return this_amount;
}

float Calculate_HistoryAmount(const nlohmann::json& performance, float& this_amount)
{
    this_amount = 40000;
    if (performance["audience"] > 30)
    {
        this_amount += 1000 * (performance["audience"].get<int>() - 30);
    }
    return this_amount;
}

float Calculate_PastoralAmount(const nlohmann::json& performance, float& this_amount)
{
    this_amount = 40000;
    if (performance["audience"] > 30)
    {
        this_amount += 1000 * (performance["audience"].get<int>() - 30);
    }
    return this_amount;
}

int Calculate_VolumeCredits(PlayType playType,const nlohmann::json& performance, int& volume_credits)
{
    switch (playType)
    {
    case PlayType::COMEDY:
    {   volume_credits += std::max(performance["audience"].get<int>() - 30, 0);
    // add extra credit for every five comedy attendees
        volume_credits += performance["audience"].get<int>() / 5;
        return volume_credits;
    }
    default:
    {
        volume_credits += std::max(performance["audience"].get<int>() - 30, 0);
        return volume_credits;
    }
    }
}

std::string statement(
    const nlohmann::json& invoice,
    const nlohmann::json& plays)
{
    float total_amount = 0;
    int volume_credits = 0;

    // this creates a new locale based on the current application default
    // (which is either the one given on startup, but can be overriden with
    // std::locale::global) - then extends it with an extra facet that
    // controls numeric output.
    const std::locale comma_locale(std::locale(), new comma_numpunct());

    std::stringstream result;
    result.imbue(comma_locale);
    result.precision(2);
    result << "Statement for " << invoice["customer"].get<std::string>() << '\n';

    for( const nlohmann::json& performance : invoice["performances"])
    {
        float this_amount = 0;
        const nlohmann::json& play = plays[performance["playID"].get<std::string>()];
        PlayType playType = getPlayType(play["type"]);
        
         switch (playType)
        {
        case PlayType::TRAGEDY:
        {
            this_amount = Calculate_TragedyAmount(performance, this_amount);
            volume_credits = Calculate_VolumeCredits(playType, performance, volume_credits);
            break;
        }
            
        case PlayType::COMEDY:
        {
            this_amount = Calculate_ComedyAmount(performance, this_amount);
            volume_credits = Calculate_VolumeCredits(playType, performance, volume_credits);
            break;
        }
        case PlayType::HISTORY:
        {
            this_amount = Calculate_HistoryAmount(performance, this_amount);
            volume_credits = Calculate_VolumeCredits(playType, performance, volume_credits);
            break;
        }
        case PlayType::PASTORAL:
        {
            this_amount = Calculate_PastoralAmount(performance, this_amount);
            volume_credits = Calculate_VolumeCredits(playType, performance, volume_credits);
            break;
        }
        default:
        {
            throw std::domain_error("unknown type: " + play["type"].get<std::string>());
            break;
        }
        }

         total_amount += this_amount;

        // print line for this order
        result << " " << play["name"].get<std::string>() << ": " << "$" << std::fixed << (this_amount/100) <<
            " (" << performance["audience"] << " seats)\n";
        
    }
    result << "Amount owed is " << "$" << std::fixed << (total_amount/100.0f) << "\n";
    result << "You earned " << volume_credits << " credits";
    return result.str();
}

