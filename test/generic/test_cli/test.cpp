#include <unity.h>
#include <cli.hpp>
#include <uart2.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    // initialized during construction
    TEST_ASSERT_TRUE(uart2::rxBuffer.isEmpty());
    TEST_ASSERT_TRUE(uart2::txBuffer.isEmpty());
    TEST_ASSERT_EQUAL(0, uart2::commandCount());

    // reinitialize after any error...
    uart2::rxBuffer.push(0xAA);
    uart2::txBuffer.push(0xBB);
    uart2::commandCounter = 5;
    uart2::initialize();
    TEST_ASSERT_TRUE(uart2::rxBuffer.isEmpty());
    TEST_ASSERT_TRUE(uart2::txBuffer.isEmpty());
    TEST_ASSERT_EQUAL(0, uart2::commandCount());
}

void test_rx() {
    uart2::initialize();
    uart2::mockReceivedChar = '?';
    uart2::rxNotEmpty();
    TEST_ASSERT_EQUAL(1, uart2::rxBuffer.getLevel());
    TEST_ASSERT_EQUAL(0, uart2::commandCount());

    uart2::mockReceivedChar = '\n';
    uart2::rxNotEmpty();
    TEST_ASSERT_EQUAL(2, uart2::rxBuffer.getLevel());
    TEST_ASSERT_EQUAL(1, uart2::commandCount());

    char receivedData[32]{};
    uart2::read(receivedData);
    TEST_ASSERT_EQUAL_STRING("?", receivedData);
    TEST_ASSERT_TRUE(uart2::rxBuffer.isEmpty());
    TEST_ASSERT_EQUAL(0, uart2::commandCount());
}

void test_tx() {
    uart2::initialize();
    uart2::send("12\n");
    TEST_ASSERT_EQUAL(3, uart2::txBuffer.getLevel());
    uart2::txEmpty();
    TEST_ASSERT_EQUAL('1', uart2::mockTransmittedChar);
    TEST_ASSERT_EQUAL(2, uart2::txBuffer.getLevel());
    uart2::txEmpty();
    TEST_ASSERT_EQUAL('2', uart2::mockTransmittedChar);
    TEST_ASSERT_EQUAL(1, uart2::txBuffer.getLevel());
    uart2::txEmpty();
    TEST_ASSERT_EQUAL('\n', uart2::mockTransmittedChar);
    TEST_ASSERT_EQUAL(0, uart2::txBuffer.getLevel());

    const uint8_t testSendData[4]{'1', '2', '3', '4'};

    uart2::send(testSendData, 2);
    TEST_ASSERT_EQUAL(2, uart2::txBuffer.getLevel());
    uart2::txEmpty();
    TEST_ASSERT_EQUAL('1', uart2::mockTransmittedChar);
    TEST_ASSERT_EQUAL(1, uart2::txBuffer.getLevel());
    uart2::txEmpty();
    TEST_ASSERT_EQUAL('2', uart2::mockTransmittedChar);
    TEST_ASSERT_EQUAL(0, uart2::txBuffer.getLevel());
}

void test_countArguments() {
    char commandLine[] = "command arg1 arg2 arg3";
    TEST_ASSERT_EQUAL(3, cli::countArguments(commandLine));

    char commandLine2[] = "command";
    TEST_ASSERT_EQUAL(0, cli::countArguments(commandLine2));

    char commandLine3[] = "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF---- ----";
    TEST_ASSERT_EQUAL(0, cli::countArguments(commandLine3));

    // Edge case : more arguments than maxNmbrOfArguments
    char commandLine4[] = "123 456 789 abc def hij";
    TEST_ASSERT_EQUAL(cliCommand::maxNmbrOfArguments, cli::countArguments(commandLine4));
}

void test_getSeparatorPosition() {
    // Normal case
    char commandLine[] = "command arg1 arg2 arg3";
    TEST_ASSERT_EQUAL(7, cli::getSeparatorPosition(commandLine, 0));
    TEST_ASSERT_EQUAL(12, cli::getSeparatorPosition(commandLine, 1));
    TEST_ASSERT_EQUAL(17, cli::getSeparatorPosition(commandLine, 2));
    TEST_ASSERT_EQUAL(-1, cli::getSeparatorPosition(commandLine, 3));

    // No arguments -> no separators
    char commandLine2[] = "command";
    TEST_ASSERT_EQUAL(-1, cli::getSeparatorPosition(commandLine2, 0));

    // Edge case : input longer than maxCommandLineLength
    char commandLine3[] = "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF---- ----";
    TEST_ASSERT_EQUAL(-1, cli::getSeparatorPosition(commandLine3, 0));
}

void test_getSegment() {
    char segment[cliCommand::maxArgumentLength]{};

    char commandLine1[] = "command arg1 arg2";
    TEST_ASSERT_EQUAL(2, cli::countArguments(commandLine1));

    cli::getSegment(segment, commandLine1, 0);
    TEST_ASSERT_EQUAL_STRING("command", segment);
    cli::getSegment(segment, commandLine1, 1);
    TEST_ASSERT_EQUAL_STRING("arg1", segment);
    cli::getSegment(segment, commandLine1, 2);
    TEST_ASSERT_EQUAL_STRING("arg2", segment);

    char commandLine2[] = "command";
    TEST_ASSERT_EQUAL(0, cli::countArguments(commandLine2));
    cli::getSegment(segment, commandLine2, 0);
    TEST_ASSERT_EQUAL_STRING("command", segment);
    cli::getSegment(segment, commandLine2, 1);
    TEST_ASSERT_EQUAL_STRING("", segment);

    char commandLine3[] = " arg1";
    TEST_ASSERT_EQUAL(1, cli::countArguments(commandLine3));
    cli::getSegment(segment, commandLine3, 0);
    TEST_ASSERT_EQUAL_STRING("", segment);
    cli::getSegment(segment, commandLine3, 1);
    TEST_ASSERT_EQUAL_STRING("arg1", segment);

    char commandLine4[] = "command ";
    TEST_ASSERT_EQUAL(1, cli::countArguments(commandLine4));
    cli::getSegment(segment, commandLine4, 0);
    TEST_ASSERT_EQUAL_STRING("command", segment);
    cli::getSegment(segment, commandLine4, 1);
    TEST_ASSERT_EQUAL_STRING("", segment);
}

void test_parseCommandLine() {
    char testCommandLine[] = "comm arg1 arg2 arg3";
    cliCommand testCommand;
    cli::parseCommandLine(testCommand, testCommandLine);

    TEST_ASSERT_EQUAL(3, testCommand.nmbrOfArguments);
    TEST_ASSERT_EQUAL_STRING("comm", testCommand.commandAsString);
    TEST_ASSERT_EQUAL_STRING("arg1", testCommand.arguments[0]);
    TEST_ASSERT_EQUAL_STRING("arg2", testCommand.arguments[1]);
    TEST_ASSERT_EQUAL_STRING("arg3", testCommand.arguments[2]);
    TEST_ASSERT_EQUAL_STRING("", testCommand.arguments[3]);
}

void test_hash() {
    TEST_ASSERT_EQUAL(65, cli::hash("A"));
    TEST_ASSERT_EQUAL(16705, cli::hash("AA"));
    TEST_ASSERT_EQUAL(1751477360, cli::hash("help"));
}

void test_toLowerCase() {
    TEST_ASSERT_EQUAL('a', cli::toLowerCase('A'));
    TEST_ASSERT_EQUAL('z', cli::toLowerCase('Z'));
    TEST_ASSERT_EQUAL('a', cli::toLowerCase('a'));
    TEST_ASSERT_EQUAL('z', cli::toLowerCase('z'));
    TEST_ASSERT_EQUAL('0', cli::toLowerCase('0'));

    char testString[] = "HeLLo WoRLd!";
    cli::toLowerCase(testString);
    TEST_ASSERT_EQUAL_STRING("hello world!", testString);

    char emptyString[] = "";
    cli::toLowerCase(emptyString);
    TEST_ASSERT_EQUAL_STRING("", emptyString);

    char longString[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    cli::toLowerCase(longString);
    TEST_ASSERT_EQUAL_STRING("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789", longString);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_rx);
    RUN_TEST(test_tx);
    RUN_TEST(test_countArguments);
    RUN_TEST(test_getSeparatorPosition);
    RUN_TEST(test_getSegment);
    RUN_TEST(test_parseCommandLine);
    RUN_TEST(test_hash);
    RUN_TEST(test_toLowerCase);
    UNITY_END();
}