import requests
import json
import time

url = 'http://rscpi.local/exec'
data = {
	'command': '',
	'expect_response': True,
    "response_timeout": 3000
}

data['command']= '*IDN?\n'
r = requests.post(url, data=json.dumps(data))
print(r.text)

# for i in range(0, 5):
# 	data['command'] = f'DISPLAY:TEXT \'TESTING: {i}/10\'\n'
# 	r = requests.post(url, data=json.dumps(data))
# 	time.sleep(1)

# data['command'] = 'DISPLAY:TEXT \'Done\'\n'

# r = requests.post(url, data=json.dumps(data))