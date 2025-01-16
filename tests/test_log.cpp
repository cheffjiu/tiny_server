#include <gtest/gtest.h>
#include "logger.h"  // 导入待测函数所在的头文件

// TEST(LoggerTest, ConstructorAndDestructor) {
//   Logger& logger = Logger::GetInstance();
//   // 验证构造函数是否正确创建线程
//   EXPECT_TRUE(logger.isRunning());
//   EXPECT_EQ(logger.getLogLevel(), Logger::INFO);
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}