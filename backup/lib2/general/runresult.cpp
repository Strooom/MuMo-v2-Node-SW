#include "runresult.h"

const char* toString(const runResult result) {
    switch (result) {
        case runResult::ready:
            return "ready";
        case runResult::busy:
            return "busy";
        default:
            return "unknown";
    }
}