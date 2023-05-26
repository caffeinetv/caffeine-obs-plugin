#pragma once

#include "policy.hpp"

#include "test-helper.hpp"

using namespace caff;

void test_oneHashtag()
{
	std::string hashtags = "#something";
	std::vector<std::string> actualResult = caff::splitHashtags(hashtags);
	std::vector<std::string> expectedResult = {"something"};

	ASSERT_EQ(actualResult, expectedResult);

	hashtags = "something";
	actualResult = caff::splitHashtags(hashtags);
	expectedResult = {"something"};
	ASSERT_EQ(actualResult, expectedResult);
}

void test_moreThanOneHashtags()
{
	std::string hashtags = "#something #test";
	std::vector<std::string> actualResult = caff::splitHashtags(hashtags);
	std::vector<std::string> expectedResult = {"something", "test"};

	ASSERT_EQ(actualResult, expectedResult);

	hashtags = "#something#test#test1";
	actualResult = caff::splitHashtags(hashtags);
	expectedResult = {"something", "test", "test1"};
	ASSERT_EQ(actualResult, expectedResult);

	hashtags = "test1 test2";
	actualResult = caff::splitHashtags(hashtags);
	expectedResult = {"test1", "test2"};
	ASSERT_EQ(actualResult, expectedResult);
}

void run_policy_tests()
{
	test_moreThanOneHashtags();
	test_oneHashtag();
	std::cout << "Successfully ran split hashtag tests" << std::endl;
}
