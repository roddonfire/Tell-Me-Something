import requests
import json

db_url = "http://iesc-s1.mit.edu/608dev/sandbox/adchurch/final/database.py"

r = requests.get(db_url)
data = r.json()
table_string = ""
for result in data:
    other_string = '''<tr>
        <td>''' + str(result['id']) + '''</th>
        <td>''' + str(result['time']) + '''</th> 
        <td>''' + str(result['result']) + '''</th>
    </tr>'''
    table_string = other_string + table_string

print(table_string)
