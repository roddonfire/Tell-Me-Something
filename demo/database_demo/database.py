import sqlite3
import requests
import datetime
import json

def request_handler(request):
    # just come up with name of database
    example_db = "__HOME__/final/esp_instructions.db"
    # connect to that database (will create if it doesn't already exist)
    conn = sqlite3.connect(example_db)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)

    c.execute('''CREATE TABLE IF NOT EXISTS esp_instructions (request_id integer primary key AUTOINCREMENT, esp_id int, sensor text, instruction text, result text, request_time timestamp, completed int, completed_time timestamp);''')

    if request['method'] == 'POST':
        esp = request['form']['esp']
        sensor = request['form']['sensor']
        instruction = request['form']['instruction']
        c.execute('''INSERT into esp_instructions VALUES (?,?,?,?,?,?,?,?);''', (None, esp, sensor, instruction, '', datetime.datetime.now(), 1, None))
        # print(c.execute('''SELECT * FROM esp_instructions''').fetchall())
        conn.commit()
        conn.close()
        return
    else: # get request
        result = c.execute('''SELECT * FROM esp_instructions WHERE completed = 1;''').fetchall()
        result_list = []
        # print(result)
        for req in result:
            result_list.append({'id':req[0], 'time':req[5], 'result':req[4]})
        return json.dumps(result_list)
