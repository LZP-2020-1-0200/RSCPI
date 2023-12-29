import requests
import json
import time

url = 'http://rscpi.local/exec'
data = {
	'command': '',
	'expect_response': False,
}

for i in range(0, 10):
	data['command'] = f'DISPLAY:TEXT \'TESTING: {i}/10\'\n'
	r = requests.post(url, data=json.dumps(data))
	time.sleep(2)

data['command'] = 'DISPLAY:TEXT \'Done\'\n'

r = requests.post(url, data=json.dumps(data))