# The Homeyduino network protocol

Communication between Homey and Homeyduino devices happens using TCP and UDP packets on port ```46639```. UDP is used for device discovery while general communication is done using ```JSON``` data sent and received using ```HTTP``` over ```TCP```.

# Device discovery
Devices can be discovered by sending an empty UDP broadcast packet on port ```46639```. Available devices will answer with a ```JSON``` object describing the device. This is the same ```JSON``` object returned when accessing the device index ```/``` using ```HTTP``` over ```TCP``` on port ```46639```.

If ```UDP``` broadcasting is not available devices can still be manually added by querying the device information over ```TCP``` by manually entering the IP address of the device into the Homey application.

## The device information JSON object

Example:
```
{
	"id": "esp32-demo",
	"type": "homeyduino",
	"class": "sensor",
	"rc": {
		"arch": "esp32",
		"numDigitalPins": "40",
		"numAnalogInputs": "16"
	},
	"master": {
		"host": "192.168.1.11",
		"port": 42419
	},
	"api": [
			{
				"name": "measure_humidity",
				"type": "cap"
			},
			{
				"name": "measure_temperature",
				"type": "cap"
			},
			{
				"name": "onoff",
				"type": "cap"
			},
			{
				"name": "led1",
				"type": "act"
			},
			{
				"name": "led2",
				"type": "act"
			}
		]
	}
}
```

| Field             | Type      | Description                                                                               | Required | Default value       |
|-------------------|-----------|-------------------------------------------------------------------------------------------|----------|---------------------|
| id                | text      | Unique device identifier string                                                           | Yes      | (device specific)   |
| type              | text      | Device type identifier string                                                             | Yes      | "homeyduino"        |
| class             | text      | Device class identifier string                                                            | Yes      | "other"             |
| rc                | object    | When set to true this field signals that the device supports the remote configuration API | No       | null                |
| - arch            | text      | CPU architecture                                                                          | (Yes)    | "unknown"           |
| - numDigitalPins  | number    | Number of digital pins on board                                                           | (Yes)    | 0                   |
| - numAnalogInputs | number    | Number of analog inputs on board                                                          | (Yes)    | 0                   |
| master            | object    | Information about the devices current master                                              | Yes      |                     |
|   - host          | text      | IP address                                                                                | Yes      | 0.0.0.0             |
|   - port          | number    | Port number                                                                               | Yes      | 9999                |
| api               | list***   | List of API calls supported by the device                                                 | Yes      | []                  |
|                   |           |                                                                                           |          |                     |
|                   |           |                                                                                           |          |                     |
|                   |           |                                                                                           |          |                     |
