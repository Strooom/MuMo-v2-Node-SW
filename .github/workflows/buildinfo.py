import subprocess
import os
import sys
import shutil
from datetime import datetime

Import("env")

latest_release_tag = subprocess.run(["git", "describe", "--tags"], stdout=subprocess.PIPE, text=True)
latest_release_tag = latest_release_tag.stdout.strip()
print ("\033[93;1;4mLatest Release Tag     : " + latest_release_tag + "\033[0m")

# determine last release version
latest_release_tag_parts = latest_release_tag.split("-")
latest_release_semver_incl_v = latest_release_tag_parts[0]
# number_of_commits_ahead = latest_release_tag_parts[1]
latest_release_semver = latest_release_semver_incl_v[1:]
latest_release_digits = latest_release_semver.split(".")
latest_release_main = latest_release_digits[0]
latest_release_minor = latest_release_digits[1]
latest_release_patch = latest_release_digits[2]
print ("\033[93;1;4mLatest Release Main    : " + latest_release_main + "\033[0m")
print ("\033[93;1;4mLatest Release Minor   : " + latest_release_minor + "\033[0m")
print ("\033[93;1;4mLatest Release Patch   : " + latest_release_patch + "\033[0m")

# determine current commit hash
current_commit_hash = subprocess.run(["git", "rev-parse", "--short", "HEAD"], stdout=subprocess.PIPE, text=True)
current_commit_hash = current_commit_hash.stdout.strip()
print ("\033[93;1;4mCurrent Commit Hash     : " + current_commit_hash + "\033[0m")
# print ("\033[93;1;4mNumber of Commits ahead : " + number_of_commits_ahead + "\033[0m")

# determine the build timstamp
build_timestamp = datetime.now().strftime("%Y-%b-%d %H:%M:%S")
print ("\033[93;1;4mBuild Timestamp        : " + build_timestamp + "\033[0m")

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
include_file.close()
