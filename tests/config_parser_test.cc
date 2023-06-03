#include "gtest/gtest.h"
#include "config_parser.h"

class NginxConfigTestFixture : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
    NginxConfigStatement config_statement; 
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

TEST_F(NginxConfigTestFixture, CommentingConfig) {
  bool success = parser.Parse("test_configs/comment_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, DoubleSlashConfig) {
  bool failure = parser.Parse("test_configs/double_slash_config", &out_config);

  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigTestFixture, WrongCharAfterQuote) {
  bool failure = parser.Parse("test_configs/incorrect_char_after_quote_config", &out_config);

  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigTestFixture, UnmatchedQuote) {
  bool failure = parser.Parse("test_configs/unmatched_quote_config", &out_config);

  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigTestFixture, ExtraSemicolon) {
  bool failure = parser.Parse("test_configs/extra_semicolon_config", &out_config);

  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigTestFixture, BadFileConfig) {
  bool failure = parser.Parse("test_configs/bad_file_config", &out_config);

  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigTestFixture, MismatchedBrackets) {
  bool failure = parser.Parse("test_configs/mismatched_brackets_config", &out_config);

  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigTestFixture, ToStringTest) {
  bool success = parser.Parse("test_configs/example_config", &out_config);

  out_config.ToString(1);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, ToStringConfigStatement) {
  bool success = parser.Parse("test_configs/example_config", &out_config);

  config_statement.ToString(1);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigTestFixture, GetCrudPathTest) {
  bool success = parser.Parse("test_configs/handler_config", &out_config);
  EXPECT_TRUE(success);

  std::string paths = out_config.get_crud_path();
  ASSERT_EQ(paths, "/api");
}

TEST_F(NginxConfigTestFixture, GetCrudArgsTest) {
  bool success = parser.Parse("test_configs/handler_config", &out_config);
  EXPECT_TRUE(success);

  std::unordered_map<std::string,std::string> args_map = out_config.get_crud_args();

  // Check if the map contains data_path arg
  auto it = args_map.find("data_path");
  bool hasKey = it != args_map.end();

  ASSERT_TRUE(hasKey);
  ASSERT_EQ(args_map["data_path"], "/foo/bar");
}

TEST_F(NginxConfigTestFixture, GetHealthPathTest) {
  bool success = parser.Parse("test_configs/health_config", &out_config);
  EXPECT_TRUE(success);

  std::string paths = out_config.get_health_path();
  ASSERT_EQ(paths, "/health");
}

TEST_F(NginxConfigTestFixture, GetSleepPathTest) {
  bool success = parser.Parse("test_configs/sleep_config", &out_config);
  EXPECT_TRUE(success);

  std::string paths = out_config.get_sleep_path();
  ASSERT_EQ(paths, "/sleep");
}

TEST_F(NginxConfigTestFixture, GetMarkdownPathTest) {
  bool success = parser.Parse("test_configs/markdown_config", &out_config);
  EXPECT_TRUE(success);

  std::string paths = out_config.get_markdown_path();
  ASSERT_EQ(paths, "/markdown");
}

TEST_F(NginxConfigTestFixture, GetStaticPathTest) {
  bool success = parser.Parse("test_configs/static_echo_config", &out_config);
  EXPECT_TRUE(success);

  std::string paths = out_config.get_static_serving_path();
  ASSERT_EQ(paths, "/static");
}

TEST_F(NginxConfigTestFixture, GetEchoPathTest) {
  bool success = parser.Parse("test_configs/static_echo_config", &out_config);
  EXPECT_TRUE(success);

  std::string paths = out_config.get_echo_path();
  ASSERT_EQ(paths, "/echo");
}

TEST_F(NginxConfigTestFixture, GetStaticFilePathsTest) {
  bool success = parser.Parse("test_configs/sample_everything_config", &out_config);
  EXPECT_TRUE(success);

  std::unordered_map<std::string,std::string> static_paths = out_config.get_static_file_path();
  ASSERT_EQ(static_paths["/static/index.html"], "static/index.html");
  ASSERT_EQ(static_paths["/static/screenshot1.png"], "static/screenshot1.png");
  ASSERT_EQ(static_paths["/static/relative_file_path.html"], "relative_file_path.html");
}