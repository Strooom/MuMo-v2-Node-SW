Example of Rx msg

mumo-v2-006
networkKey : 8DD6FEB76D7754A78538BA5089A834AA
applicationKey : E54E4411F4FE8955904197C8D824BC2C

mac payload :                   2261D02E6DC32E0495279335D16B41E109C11CB7D5DF80232FD81280DE42BE47C964B3
raw payload : 6011990b2600b000002261d02e6dc32e0495279335d16b41e109c11cb7d5df80232fd81280de42be47c964b382879a3f

manual parsing : 
60 : unconfirmed downlink
11990b26 : devAddr
00 : fctrl
b000 : fcnt 176
00 : fport
2261d02e6dc32e0495279335d16b41e109c11cb7d5df80232fd81280de42be47c964b3 : framePayload
82879a3f : mic

{
  "name": "ns.down.data.schedule.attempt",
  "time": "2024-03-21T12:17:41.019148905Z",
  "identifiers": [
    {
      "device_ids": {
        "device_id": "mumo-v2-006",
        "application_ids": {
          "application_id": "tests-lora-e5"
        },
        "dev_eui": "70B3D57ED005E7DA",
        "dev_addr": "260B9911"
      }
    }
  ],
  "data": {
    "@type": "type.googleapis.com/ttn.lorawan.v3.DownlinkMessage",
    "raw_payload": "YBGZCyYAsAAAImHQLm3DLgSVJ5M10WtB4QnBHLfV34AjL9gSgN5CvkfJZLOCh5o/",
    "payload": {
      "m_hdr": {
        "m_type": "UNCONFIRMED_DOWN"
      },
      "mic": "goeaPw==",
      "mac_payload": {
        "f_hdr": {
          "dev_addr": "260B9911",
          "f_ctrl": {},
          "f_cnt": 176
        },
        "frm_payload": "ImHQLm3DLgSVJ5M10WtB4QnBHLfV34AjL9gSgN5CvkfJZLM=",
        "full_f_cnt": 176
      }
    },
    "request": {
      "downlink_paths": [
        {
          "uplink_token": "CiIKIAoUZXVpLWFjMWYwOWZmZmUwOGQ3MjESCKwfCf/+CNchENDtgv0NGgwI5MrwrwYQ8YCipAIggLmo3pjPhwE="
        }
      ],
      "rx1_delay": 1,
      "rx1_data_rate": {
        "lora": {
          "bandwidth": 125000,
          "spreading_factor": 7,
          "coding_rate": "4/5"
        }
      },
      "rx1_frequency": "868100000",
      "rx2_data_rate": {
        "lora": {
          "bandwidth": 125000,
          "spreading_factor": 12,
          "coding_rate": "4/5"
        }
      },
      "rx2_frequency": "869525000",
      "priority": "HIGHEST",
      "frequency_plan_id": "EU_863_870_TTN"
    },
    "correlation_ids": [
      "gs:uplink:01HSGF4485S5BQVTS8TD29MKAB",
      "ns:downlink:01HSGF44MVJ77JBCX8KRK4YJH9",
      "ns:transmission:01HSGF44MVVYYQYCN8NKGZCVF3"
    ]
  },
  "correlation_ids": [
    "gs:uplink:01HSGF4485S5BQVTS8TD29MKAB",
    "ns:downlink:01HSGF44MVJ77JBCX8KRK4YJH9",
    "ns:transmission:01HSGF44MVVYYQYCN8NKGZCVF3"
  ],
  "origin": "ip-10-100-12-27.eu-west-1.compute.internal",
  "context": {
    "tenant-id": "CgN0dG4="
  },
  "visibility": {
    "rights": [
      "RIGHT_APPLICATION_TRAFFIC_READ"
    ]
  },
  "unique_id": "01HSGF44MVS26V19938P8V6R2V"
}


Example 2 : 

                  0E44D8985C2F296FE7EB2FFCE788D9F05D7BC5F8784FDF7402CB6931AEA200E264CC03
6011990b2600b100000e44d8985c2f296fe7eb2ffce788d9f05d7bc5f8784fdf7402cb6931aea200e264cc03e16e3580
{
  "name": "ns.down.data.schedule.attempt",
  "time": "2024-03-21T12:21:40.978064191Z",
  "identifiers": [
    {
      "device_ids": {
        "device_id": "mumo-v2-006",
        "application_ids": {
          "application_id": "tests-lora-e5"
        },
        "dev_eui": "70B3D57ED005E7DA",
        "dev_addr": "260B9911"
      }
    }
  ],
  "data": {
    "@type": "type.googleapis.com/ttn.lorawan.v3.DownlinkMessage",
    "raw_payload": "YBGZCyYAsQAADkTYmFwvKW/n6y/854jZ8F17xfh4T990AstpMa6iAOJkzAPhbjWA",
    "payload": {
      "m_hdr": {
        "m_type": "UNCONFIRMED_DOWN"
      },
      "mic": "4W41gA==",
      "mac_payload": {
        "f_hdr": {
          "dev_addr": "260B9911",
          "f_ctrl": {},
          "f_cnt": 177
        },
        "frm_payload": "DkTYmFwvKW/n6y/854jZ8F17xfh4T990AstpMa6iAOJkzAM=",
        "full_f_cnt": 177
      }
    },
    "request": {
      "downlink_paths": [
        {
          "uplink_token": "CiIKIAoUZXVpLWFjMWYwOWZmZmUwOGQ3MjESCKwfCf/+CNchEKiiuO8OGgwI1MzwrwYQ/72mjwIgwIjMz5bWhwE="
        }
      ],
      "rx1_delay": 1,
      "rx1_data_rate": {
        "lora": {
          "bandwidth": 125000,
          "spreading_factor": 7,
          "coding_rate": "4/5"
        }
      },
      "rx1_frequency": "868100000",
      "rx2_data_rate": {
        "lora": {
          "bandwidth": 125000,
          "spreading_factor": 12,
          "coding_rate": "4/5"
        }
      },
      "rx2_frequency": "869525000",
      "priority": "HIGHEST",
      "frequency_plan_id": "EU_863_870_TTN"
    },
    "correlation_ids": [
      "gs:uplink:01HSGFBEJS7Y7GFKMRF62GT3ZR",
      "ns:downlink:01HSGFBEZJXYADETNZPZAQ4RF4",
      "ns:transmission:01HSGFBEZJ4706TZVPZFQN5MM4"
    ]
  },
  "correlation_ids": [
    "gs:uplink:01HSGFBEJS7Y7GFKMRF62GT3ZR",
    "ns:downlink:01HSGFBEZJXYADETNZPZAQ4RF4",
    "ns:transmission:01HSGFBEZJ4706TZVPZFQN5MM4"
  ],
  "origin": "ip-10-100-12-27.eu-west-1.compute.internal",
  "context": {
    "tenant-id": "CgN0dG4="
  },
  "visibility": {
    "rights": [
      "RIGHT_APPLICATION_TRAFFIC_READ"
    ]
  },
  "unique_id": "01HSGFBEZJEC0YJ2JD8N9WK3ED"
}