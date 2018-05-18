import requests
import json
import base64
import numpy as np
import scipy.io.wavfile as spw

db_url = "http://iesc-s1.mit.edu/608dev/sandbox/adchurch/final/database.py"

def write_html_table(data):
	table_string = ""
	for result in data:
		other_string = ""
		if len(result['result']) < 3000:
			other_string = '''<tr>
				<td>''' + str(result['id']) + '''</td>
				<td>''' + str(result['time']) + '''</td> 
				<td>''' + str(result['result']) + '''</td>
			</tr>'''
		else: # audio data
			audio_data = []
			s = str(result['result'])
			try:
				audio_data = base64.b64decode(s + '=========')
			except:
				audio_data = "error"
			if audio_data != "error":
				# write wav file
				int_array = []
				for b in audio_data:
					int_array.append((b + 20) % 256)

				to_wav = np.array(int_array, np.dtype(np.uint8))
				path = '__HOME__/finalProject/test.wav'
				spw.write(path, 8000, to_wav)
				wav_string = ""
				with open(path, "rb") as mywav:
					wav_string = base64.b64encode(mywav.read()).decode('ascii')


				other_string = '''<tr>
					<td>''' + str(result['id']) + '''</td>
					<td>''' + str(result['time']) + '''</td>
					<td>
						<audio controls>
						<source src="data:audio/wav;base64, {0}" >
							Audio from ESP32
						</audio>
					</td>
				</tr>'''.format(wav_string)
			else: # error decoding audio
				other_string = '''<tr>
					<td>''' + str(result['id']) + '''</td>
					<td>''' + str(result['time']) + '''</td> 
					<td>Error decoding audio.</td>
				</tr>'''
		table_string = other_string + table_string
	
	return table_string
	

def request_handler(request):
	
	with open('__HOME__/finalProject/RH1_final.html', 'r') as myfile:
		
		if 'request' not in request['args']:
			html = myfile.read()
			r = requests.get(db_url)
			data = r.json()
			table_string = write_html_table(data)
			
			
			# insert table into html
			# print(html.index('<!--TABLE-->'))
			return html.replace("<!--TABLE-->", table_string)
		
		if request['values']['request'] == 'UpdateMe':
			html = myfile.read() #string holding html of the site
			
			r = requests.get(db_url)
			data = r.json()
			table_string = write_html_table(data)
			
			
			# insert table into html
			# print(html.index('<!--TABLE-->'))
			return html.replace("<!--TABLE-->", table_string)
		else: # tell me request just sent
			# send post request to database with info from request the user just made
			if 'ESPNum' in request['values'].keys() and 'RequestOption' in request['values'].keys(): # if new result to be posted
				esp = request['values']['ESPNum']
				sensor = request['values']['RequestOption']
				option = request['values']['options[]']
				param1 = ""
				param2 = ""
				instruction = ""
				if 'Parameter1' in request['values'].keys():
					param1 = request['values']['Parameter1']
				if 'Parameter2' in request['values'].keys():
					param2 = request['values']['Parameter2']
				# parse from server format to esp-friendly format
				if option == 'aSingle':
					instruction = "accel$single$" + param1
				elif option == 'aAvg':
					instruction = "accel$avg$" + param1 + ',' + param2
				elif option == 'gSingle':
					instruction = "gyro$single$" + param1
				elif option == 'gAvg':
					instruction = 'gyro$avg$' + param1 + ',' + param2
				elif option == 'tSingle':
					instruction = 'temp$single'
				elif option == 'tAvg':
					instruction = 'temp$avg$' + param1
				elif option == 'con':
					instruction = 'status$con'
				elif option == 'sat':
					instruction = 'status$sat'
				elif option == 'loc':
					instruction = 'loc'
				elif option == 'time':
					instruction = 'time'
				elif option == 'uSingle':
					instruction = 'single'
				elif option == 'uAvg':
					instruction = 'avg$' + param1
				elif option == 'rec':
					instruction = 'rec$' + param1
				elif option == 'tOn':
					instruction = 'set$1'
				elif option == 'tOff':
					instruction = 'set$0'
				elif option == 'startF':
					instruction = 'flash$1$' + param1
				elif option == 'stopF':
					instruction = 'flash$0'
				elif option == 'volt':
					instruction = 'volt'
				elif option == 'sleep':
					instruction = 'sleep$' + param1

				payload = {'esp':esp, 'sensor':sensor, 'instruction':instruction, 'update': 'F'}
				requests.post(db_url, data=payload)


			html = myfile.read() #string holding html of the site
			
			r = requests.get(db_url)
			data = r.json()
			table_string = write_html_table(data)
			
			# insert table into html
			# print(html.index('<!--TABLE-->'))
			return html.replace("<!--TABLE-->", table_string)
		
		
	
