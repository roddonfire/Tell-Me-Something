import sqlite3
import datetime


visits_db = '__HOME__/final/power_info.db'


def request_handler(request):
    # connect to that database (will create if it doesn't already exist)
    conn = sqlite3.connect(visits_db)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)

    c.execute('''CREATE TABLE IF NOT EXISTS power_info (request_id integer primary key AUTOINCREMENT, esp_id int, request_time timestamp);''')
    
    if request['method'] == "GET":
        # save time of request
        if 'esp_id' in request['args']:
            esp = int(request['values']['esp_id'])
            c.execute('''INSERT into power_info VALUES (?,?,?); ''', (None, esp, datetime.datetime.now()))
            conn.commit()
            conn.close()
        elif 'check' in request['args']:
            result = c.execute('''SELECT * FROM power_info;''').fetchall()
            output = ""
            for t in result:
                output += "Entry: " + str(t[0]) + "; Time: " + str(t[2]) + "\n"
            return output
        elif 'drop_all' in request['args']:
            c.execute('''DROP TABLE power_info;''')
            conn.commit()
            conn.close()

    else:
        conn.commit()  # commit commands
        conn.close()  # close connection to database
        return "bad request"
