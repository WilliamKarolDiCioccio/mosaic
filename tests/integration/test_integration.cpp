#include <gtest/gtest.h>

#include <mosaic/core/application.hpp>

class TestApplication : public mosaic::core::Application
{
   public:
    TestApplication() : Application("TestApplication") {}
    ~TestApplication() {}

   protected:
    void onInitialize() override {}
    void onUpdate() override {}
    void onPause() override {}
    void onResume() override {}
    void onShutdown() override {}
};

TEST(IntegrationTest, ApplicationInitialization)
{
    TestApplication app;
    EXPECT_NO_THROW(app.initialize());
    EXPECT_TRUE(app.isInitialized());
}
