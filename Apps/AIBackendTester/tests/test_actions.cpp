#include <gtest/gtest.h>

#include "actions/actionconnect.hpp"
#include "actions/actiondisconnect.hpp"
#include "actions/actionping.hpp"
#include "actions/actionask.hpp"
#include "actions/actionaskstatus.hpp"
#include "actions/actionaskinterrupt.hpp"
#include "actions/actionsetconfig.hpp"

#include <sstream>
#include <vector>
#include <string>

// Helper to parse arguments like AIBackendTester::parseArguments
static std::vector<std::string> parseArgs(const std::string& line)
{
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        args.push_back(current);
    }
    return args;
}

// ---- ConnectAction tests ----

TEST(ConnectAction, Validate_ValidArgs_ReturnsTrue)
{
    ConnectAction action;
    EXPECT_TRUE(action.validate({"127.0.0.1", "9100", "abcdef1234567890abcdef1234567890"}));
}

TEST(ConnectAction, Validate_MissingArgs_ReturnsFalse)
{
    ConnectAction action;
    EXPECT_FALSE(action.validate({}));
    EXPECT_FALSE(action.validate({"127.0.0.1"}));
    EXPECT_FALSE(action.validate({"127.0.0.1", "9100"}));
}

TEST(ConnectAction, Validate_EmptyStrings_ReturnsFalse)
{
    ConnectAction action;
    EXPECT_FALSE(action.validate({"", "9100", "token"}));
    EXPECT_FALSE(action.validate({"127.0.0.1", "", "token"}));
}

TEST(ConnectAction, NameAndDescription_NotEmpty)
{
    ConnectAction action;
    EXPECT_FALSE(action.getName().empty());
    EXPECT_FALSE(action.getDescription().empty());
    EXPECT_FALSE(action.getUsage().empty());
}

// ---- DisconnectAction tests ----

TEST(DisconnectAction, Validate_NoArgs_ReturnsTrue)
{
    DisconnectAction action;
    EXPECT_TRUE(action.validate({}));
}

TEST(DisconnectAction, Validate_WithArgs_ReturnsFalse)
{
    DisconnectAction action;
    EXPECT_FALSE(action.validate({"extra"}));
}

TEST(DisconnectAction, NameAndDescription_NotEmpty)
{
    DisconnectAction action;
    EXPECT_FALSE(action.getName().empty());
    EXPECT_FALSE(action.getDescription().empty());
}

// ---- PingAction tests ----

TEST(PingAction, Validate_NoArgs_ReturnsTrue)
{
    PingAction action;
    EXPECT_TRUE(action.validate({}));
}

TEST(PingAction, Validate_ValidByteCount_ReturnsTrue)
{
    PingAction action;
    EXPECT_TRUE(action.validate({"16"}));
    EXPECT_TRUE(action.validate({"8"}));
}

TEST(PingAction, Validate_ByteCountAndTimeout_ReturnsTrue)
{
    PingAction action;
    EXPECT_TRUE(action.validate({"16", "2000"}));
}

TEST(PingAction, Validate_TooManyArgs_ReturnsFalse)
{
    PingAction action;
    EXPECT_FALSE(action.validate({"8", "1000", "extra"}));
}

TEST(PingAction, Validate_InvalidByteCount_ReturnsFalse)
{
    PingAction action;
    EXPECT_FALSE(action.validate({"0"}));
    EXPECT_FALSE(action.validate({"abc"}));
}

TEST(PingAction, Validate_InvalidTimeout_ReturnsFalse)
{
    PingAction action;
    EXPECT_FALSE(action.validate({"8", "abc"}));
}

// ---- AskAction tests ----

TEST(AskAction, Validate_NoArgs_ReturnsTrue)
{
    AskAction action;
    EXPECT_TRUE(action.validate({}));
}

TEST(AskAction, Validate_OneArg_ReturnsTrue)
{
    AskAction action;
    EXPECT_TRUE(action.validate({"Say hello"}));
}

TEST(AskAction, Validate_TooManyArgs_ReturnsFalse)
{
    AskAction action;
    EXPECT_FALSE(action.validate({"one", "two"}));
}

// ---- AskStatusAction tests ----

TEST(AskStatusAction, Validate_OneArg_ReturnsTrue)
{
    AskStatusAction action;
    EXPECT_TRUE(action.validate({"someAskId123"}));
}

TEST(AskStatusAction, Validate_NoArgs_ReturnsFalse)
{
    AskStatusAction action;
    EXPECT_FALSE(action.validate({}));
}

TEST(AskStatusAction, Validate_EmptyArg_ReturnsFalse)
{
    AskStatusAction action;
    EXPECT_FALSE(action.validate({""}));
}

// ---- AskInterruptAction tests ----

TEST(AskInterruptAction, Validate_OneArg_ReturnsTrue)
{
    AskInterruptAction action;
    EXPECT_TRUE(action.validate({"someAskId456"}));
}

TEST(AskInterruptAction, Validate_NoArgs_ReturnsFalse)
{
    AskInterruptAction action;
    EXPECT_FALSE(action.validate({}));
}

// ---- SetConfigAction tests ----

TEST(SetConfigAction, Validate_NoArgs_ReturnsTrue)
{
    SetConfigAction action;
    EXPECT_TRUE(action.validate({}));
}

TEST(SetConfigAction, Validate_OneArg_ReturnsTrue)
{
    SetConfigAction action;
    EXPECT_TRUE(action.validate({"FROM qwen3.5\nPARAMETER temperature 0.7"}));
}

TEST(SetConfigAction, Validate_TooManyArgs_ReturnsFalse)
{
    SetConfigAction action;
    EXPECT_FALSE(action.validate({"config1", "config2"}));
}

// ---- Argument parser tests ----

TEST(ArgumentParser, SimpleCommand_ReturnsSingleArg)
{
    auto args = parseArgs("help");
    ASSERT_EQ(args.size(), 1);
    EXPECT_EQ(args[0], "help");
}

TEST(ArgumentParser, CommandWithArgs_ReturnsMultipleArgs)
{
    auto args = parseArgs("connect 127.0.0.1 9100 mytoken");
    ASSERT_EQ(args.size(), 4);
    EXPECT_EQ(args[0], "connect");
    EXPECT_EQ(args[1], "127.0.0.1");
    EXPECT_EQ(args[2], "9100");
    EXPECT_EQ(args[3], "mytoken");
}

TEST(ArgumentParser, QuotedArg_TreatedAsSingleArg)
{
    auto args = parseArgs("ask \"hello world test\"");
    ASSERT_EQ(args.size(), 2);
    EXPECT_EQ(args[0], "ask");
    EXPECT_EQ(args[1], "hello world test");
}

TEST(ArgumentParser, EmptyLine_ReturnsEmpty)
{
    auto args = parseArgs("");
    EXPECT_TRUE(args.empty());
}

TEST(ArgumentParser, OnlySpaces_ReturnsEmpty)
{
    auto args = parseArgs("   ");
    EXPECT_TRUE(args.empty());
}

TEST(ArgumentParser, MultipleSpaces_TreatedAsSingleSeparator)
{
    auto args = parseArgs("ping   8   2000");
    ASSERT_EQ(args.size(), 3);
    EXPECT_EQ(args[0], "ping");
    EXPECT_EQ(args[1], "8");
    EXPECT_EQ(args[2], "2000");
}