### Versioning ###

We keep track of 2 versions :
* isVersion : the read-only properties of the current running firmware - properties are set by build script or CI/CD script
* toBeVersion : the 3-digit SemVer version that we should update to.

* If they are the same, we do not need firmware update
* We should report both versions over MQTT when requested from MQTT

As there is not that much overlap between both versions, it's maybe better to implement both in the same static class.