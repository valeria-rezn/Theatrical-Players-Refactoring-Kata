#include "statement.h"

#include "json.hpp"
#include "doctest/doctest.h"
#include "ApprovalTests.hpp"

#include <iostream>

namespace
{
    std::string get_adjacent_file(const std::string& filename)
    {
        return ApprovalTests::ApprovalTestNamer().getDirectory() + filename;
    }

    nlohmann::json read_json_file(const std::string& filename)
    {
        std::ifstream invoice_stream(filename);
        if ( ! invoice_stream.is_open())
        {
            std::cout << "Error opening file:" << filename << '\n';
            std::cout << "Error: " << strerror(errno) << '\n';
        }
        nlohmann::json result;
        invoice_stream >> result;
        return result;
    }
}

TEST_CASE("test_example_statement")
{
    auto invoice = read_json_file(get_adjacent_file("invoice.json"));
    auto plays = read_json_file(get_adjacent_file("plays.json"));

    ApprovalTests::Approvals::verify(statement(invoice, plays));
}

TEST_CASE("test_example_statement2")
{
    auto invoice = read_json_file(get_adjacent_file("invoice_new_plays.json"));
    auto plays = read_json_file(get_adjacent_file("new_plays.json"));
    ApprovalTests::Approvals::verify(statement(invoice, plays));
}

