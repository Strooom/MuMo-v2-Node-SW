import subprocess
import os
import sys
import shutil
from datetime import datetime, timezone

Import("env")


tag_hash = subprocess.run(["git", "rev-list", "--tags", "--max-count=1"],stdout=subprocess.PIPE, text=True, check=True).stdout.rstrip()
latest_release_tag = subprocess.run(["git", "describe", "--tags", tag_hash], stdout=subprocess.PIPE, text=True, check=True).stdout.rstrip()
print ("")
print ("\033[93;1mLatest Release Tag      : " + latest_release_tag + "\033[0m")

# determine last release version
latest_release_tag_parts = latest_release_tag.split("-")
latest_release_semver_incl_v = latest_release_tag_parts[0]
if latest_release_tag_parts.__len__() > 1:
    number_of_commits_ahead = latest_release_tag_parts[1]
else:
    number_of_commits_ahead = 0
latest_release_semver = latest_release_semver_incl_v[1:]
latest_release_digits = latest_release_semver.split(".")
latest_release_main = latest_release_digits[0]
latest_release_minor = latest_release_digits[1]
latest_release_patch = latest_release_digits[2]
print ("\033[93;1mLatest Release Main     : " + str(latest_release_main) + "\033[0m")
print ("\033[93;1mLatest Release Minor    : " + str(latest_release_minor) + "\033[0m")
print ("\033[93;1mLatest Release Patch    : " + str(latest_release_patch) + "\033[0m")

# determine current commit hash
current_commit_hash = subprocess.run(["git", "rev-parse", "--short", "HEAD"], stdout=subprocess.PIPE, text=True)
current_commit_hash = current_commit_hash.stdout.strip()
print ("\033[93;1mCurrent Commit Hash     : " + current_commit_hash + "\033[0m")
print ("\033[93;1mNumber of Commits ahead : " + str(number_of_commits_ahead) + "\033[0m")

# determine the build timstamp

build_timestamp = datetime.now(timezone.utc).strftime("%Y-%b-%d %H:%M:%S")
build_epoch = int(datetime.now(timezone.utc).timestamp())
print ("\033[93;1mBuild Timestamp UTC     : " + build_timestamp + "\033[0m")
print ("\033[93;1mBuild Timestamp Epoch   : " + str(build_epoch) + "\033[0m")
print ("")

# store the results in a source file, so our source code has access to it
include_file = open('lib/version/buildinfo.cpp', 'w')
include_file.write("// ##########################################################################\n")
include_file.write("// ### This file is generated by Build and Continuous Integration scripts ###\n")
include_file.write("// ###   .github/workflows/buildinfo.py for local development environment ###\n")
include_file.write("// ###   .github/workflows/testbuildrelease.yml for CI environment        ###\n")
include_file.write("// ### Changes will be overwritten on the next build                      ###\n")
include_file.write("// ##########################################################################\n")
include_file.write("\n")
include_file.write("#include <buildinfo.hpp>\n")
include_file.write("\n")
include_file.write("const buildEnvironment buildInfo::theBuildEnvironment{buildEnvironment::local};\n")
include_file.write("const buildType buildInfo::theBuildType{buildType::development};\n")
include_file.write("const int buildInfo::mainVersionDigit   = " + latest_release_main + ";\n")
include_file.write("const int buildInfo::minorVersionDigit  = " + latest_release_minor + ";\n")
include_file.write("const int buildInfo::patchVersionDigit  = " + latest_release_patch + ";\n")
include_file.write("const char* buildInfo::lastCommitTag    = \"" + current_commit_hash + "\";\n")
include_file.write("const char* buildInfo::buildTimeStamp   = \"" + build_timestamp + "\";\n")
include_file.write("const time_t buildInfo::buildEpoch      = " + str(build_epoch) + ";\n")
include_file.close()
