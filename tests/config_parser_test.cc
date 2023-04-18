#include "gtest/gtest.h"
#include "config_parser.h"

class NginxConfigTestFixture : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

TEST_F(NginxConfigTestFixture, SimpleConfig) {
  bool success = parser.Parse("test_configs/example_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, EmptyBodyConfig) {
  bool success = parser.Parse("test_configs/empty_body_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, UnmatchedBracketConfig) {
  bool fail = parser.Parse("test_configs/unmatched_bracket_config", &out_config);

  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigTestFixture, QuoteSpaceConfig) {
  bool success = parser.Parse("test_configs/quote_space_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, EscapeConfig) {
  bool success = parser.Parse("test_configs/escape_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, UnmatchedEndBracketConfig) {
  bool fail = parser.Parse("test_configs/unmatched_end_bracket_config", &out_config);

  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigTestFixture, NestedBracketConfig) {
  bool success = parser.Parse("test_configs/nested_bracket_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, PortNumberValid) {
  bool success = parser.Parse("test_configs/port_config", &out_config);

  int port = out_config.get_port_num();
  bool port_val = (port==3100);

  EXPECT_TRUE(port_val);
}

TEST_F(NginxConfigTestFixture, PortNumberInvalid) {
  bool success = parser.Parse("test_configs/bad_port_config", &out_config);

  int port = out_config.get_port_num();
  bool port_val = (port==-1);

  EXPECT_TRUE(port_val);
}
