#include <unity.h>
#include <cli.hpp>
#include <uart.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_uart_rx() {
    uart2::mockReceivedChar = '?';
    uart2::rxNotEmpty();
    TEST_ASSERT_EQUAL(0, uart2::commandCount());

    uart2::mockReceivedChar = '\n';
    uart2::rxNotEmpty();
    TEST_ASSERT_EQUAL(1, uart2::commandCount());

    char receivedData[32]{};
    uart2::read(receivedData);
    TEST_ASSERT_EQUAL_STRING("?", receivedData);
    TEST_ASSERT_EQUAL(0, uart2::commandCount());
}

void test_uart_tx() {
    uart2::initialize();        // for coverage only
    uart2::send("12");
    TEST_ASSERT_EQUAL(2, uart2::txBuffer.getLevel());
    uart2::txEmpty();
    TEST_ASSERT_EQUAL('1', uart2::mockTransmittedChar);
    TEST_ASSERT_EQUAL(1, uart2::txBuffer.getLevel());
    uart2::txEmpty();
    TEST_ASSERT_EQUAL('2', uart2::mockTransmittedChar);
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
    char segment[cliCommand::maxCommandOrArgumentLength]{};

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

void test_splitCommandLine() {
    char commandLine[] = "command arg1 arg2 arg3";
    cli::splitCommandLine(commandLine);
    TEST_ASSERT_EQUAL_STRING("command", cli::command);
    TEST_ASSERT_EQUAL_STRING("arg1", cli::arguments[0]);
    TEST_ASSERT_EQUAL_STRING("arg2", cli::arguments[1]);
    TEST_ASSERT_EQUAL_STRING("arg3", cli::arguments[2]);
    TEST_ASSERT_EQUAL_STRING("", cli::arguments[3]);

    char commandLine2[] = "command";
    cli::splitCommandLine(commandLine2);
    TEST_ASSERT_EQUAL_STRING("command", cli::command);
    TEST_ASSERT_EQUAL_STRING("", cli::arguments[0]);
}

void test_findCommandIndex() {
    char commandLine1[] = "?";
    cli::splitCommandLine(commandLine1);
    TEST_ASSERT_EQUAL(0, cli::findCommandIndex());

    char commandLine2[] = "help";
    cli::splitCommandLine(commandLine2);
    TEST_ASSERT_EQUAL(0, cli::findCommandIndex());

    char commandLine3[] = "set-devaddr 12345678";
    cli::splitCommandLine(commandLine3);
    TEST_ASSERT_EQUAL(5, cli::findCommandIndex());

    char commandLine4[] = "unknown command";
    cli::splitCommandLine(commandLine4);
    TEST_ASSERT_EQUAL(-1, cli::findCommandIndex());
}

void test_parseCommandLine() {
    char commandLine1[] = "help";
    cli::executeCommand(commandLine1);
    char commandLine2[] = "set-devaddr 12345678";
    cli::executeCommand(commandLine2);
    char commandLine3[] = "unknown command";
    cli::executeCommand(commandLine3);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_uart_rx);
    RUN_TEST(test_uart_tx);
    RUN_TEST(test_countArguments);
    RUN_TEST(test_getSeparatorPosition);
    RUN_TEST(test_getSegment);
    RUN_TEST(test_splitCommandLine);
    RUN_TEST(test_findCommandIndex);
    RUN_TEST(test_parseCommandLine);
    UNITY_END();
}