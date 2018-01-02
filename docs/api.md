# Arduino API

```#include <Homey.h>```

## Functions

| Function           | Return type   | Arguments                                         | Description                                                                               |
|--------------------|---------------|---------------------------------------------------|-------------------------------------------------------------------------------------------|
| begin              | void          | String: name                                      | Start responding to queries                                                               |
| stop               | void          | void                                              | Stop responding to queries                                                                |
| loop               | void          | void                                              | Handle queries (needs to be called from the Arduino loop function as often as possible)   |
| getName            | String: name  | void                                              | Get the current device identifier                                                         |
| setName            | void          | String: name                                      | Change the device identifier                                                              |
| getClass           | String: class | void                                              | Get the current device class                                                              |
| setClass           | void          | String: class                                     | Change the device class                                                                   |
| addAction          | bool          | String name, CallbackFunction fn                  | Add an action                                                                             |
| addCondition       | bool          | String name, CallbackFunction fn                  | Add a condition                                                                           |
| addCapability      | bool          | String name, (CallbackFunction fn)                | Add a capability                                                                          |
| removeAction       | bool          | String name                                       | Remove an action                                                                          |
| removeCondition    | bool          | String name                                       | Remove a condition                                                                        |
| removeCapability   | bool          | String name                                       | Remove a capability                                                                       |
| clear              | void          | void                                              | Remove all actions, conditions and capabilities                                           |
| trigger            | bool          | String name, (String/bool/int/float/double value) | Send a trigger to Homey                                                                   |
| setCapabilityValue | bool          | String name, String/bool/int/float/double value   | Send a capability value to Homey                                                          |
| returnNothing      | void          | void                                              | Call from within condition or capability function to set the returned value to NULL       |
| returnResult       | void          | String/bool/int/float/double value                | Call from within condition or capability function to set the returned result              |
| returnError        | void          | String message                                    | Call from within an action, condition or capability function to return an error message   |

## Variables

| Variable           | Type          | Description                                                                                                                                   |
|--------------------|---------------|-----------------------------------------------------------------------------------------------------------------------------------------------|
| value 						 | String        | The argument supplied by the Homey flow (can be read from within an action, condition or capability callback)                                 |

## Remote configuration
| Function        | Usage                                                               |
|-----------------|---------------------------------------------------------------------|
| rcEnable();     | Call this function from ```setup``` to enable remote configuration  |
| rcTriggerRun(); | Call this function from ```loop``` to handle configured RC triggers |
