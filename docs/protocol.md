# The Homeyduino network protocol

Communication between Homey and Homeyduino devices happens using TCP and UDP packets on port ```46639```. UDP is used for device discovery while general communication is done using ```JSON``` data sent and received using ```HTTP``` over ```TCP```.

# Device discovery
Devices can be discovered by sending an empty UDP broadcast packet on port ```46639```. Available devices will answer with a ```JSON``` object describing the device. This is the same ```JSON``` object returned when accessing the device index ```/``` using ```HTTP``` over ```TCP``` on port ```46639```.

If ```UDP``` broadcasting is not available devices can still be manually added by querying the device information over ```TCP``` by manually entering the IP address of the device into the Homey application.

## The device information JSON object

Example:
```
{
	"id": "example",
	"version": "0.0.0",
	"type": "homeyduino",
	"class": "sensor",
	"master": {
		"host": "0.0.0.0",
		"port": 9999
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
				"name": "example_activity",
				"type": "act"
			},
			{
				"name": "example_condition",
				"type": "con"
			}
		]
	}
}
```

| Field             | Type      | Description                                                                               | Required | Default value       |
|-------------------|-----------|-------------------------------------------------------------------------------------------|----------|---------------------|
| id                | text      | Unique device identifier string                                                           | Yes      | (device specific)   |
| version           | text      | Version of the Homeyduino Arduino library used in the device                              | Yes      |                     |  
| type              | text      | Device type identifier string                                                             | Yes      | "homeyduino"        |
| class             | text      | Device class identifier string                                                            | Yes      | "other"             |
| master            | object    | Information about the devices current master                                              | Yes      |                     |
|   - host          | text      | IP address                                                                                | Yes      | 0.0.0.0             |
|   - port          | number    | Port number                                                                               | Yes      | 9999                |
| api               | list      | List of API calls supported by the device                                                 | Yes      | []                  |

## Calling API endpoints
All API endpoints are available at URI ```/<type>/<name>```. For example an activity called "example" would be available at URI ```/act/example```.

A request may be of either a POST or a GET type. If an API endpoint does not have a stored value (which is the case for activities and conditions) then both POST and GET will yield the same result: the callback function gets called and the result is returned.
For endpoints with a stored value a GET request will return the current value of the endpoint, whereas a POST request will call the callback function.
