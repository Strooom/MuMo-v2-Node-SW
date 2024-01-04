# https://github.com/marketplace/actions/release-please-action
# https://www.conventionalcommits.org/en/v1.0.0/
# https://docs.github.com/en/repositories/releasing-projects-on-github/automatically-generated-release-notes
# https://docs.github.com/en/actions
# https://naereen.github.io/badges/

# Todo
* test that the version inside the firmware is correct, for both local builds and cloud builds

# some code to pick up the log of git since last version..
# git log v1.1.7..HEAD --no-merges --pretty=format:%Cred%s%Creset%n%b