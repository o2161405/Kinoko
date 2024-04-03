#pragma once

#include "test/Test.hh"

#include <egg/util/Stream.hh>

#include <queue>
#include <span>

namespace Test {

struct TestCase {
    std::string name;
    std::string rkgPath;
    std::string krkgPath;
    u16 targetFrame;
};

class TestDirector {
public:
    TestDirector(std::span<u8> &suiteData);
    ~TestDirector();

    void parseSuite(EGG::RamStream &stream);
    void init();
    bool calc();
    bool test(const TestData &data);
    void writeTestOutput() const;

    TestData findNextEntry();
    const TestCase &testCase() const;

    bool sync() const;

private:
    void readHeader();

    std::queue<TestCase> m_testCases;
    u16 m_curTestIdx;

    void *m_file;
    EGG::RamStream m_stream;

    u16 m_versionMajor;
    u16 m_versionMinor;
    u16 m_frameCount;
    u16 m_currentFrame;
    bool m_sync;
};

} // namespace Test
