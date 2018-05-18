import sqlite3
import datetime
import json

visits_db = '__HOME__/esp_instructions_demo.db'

def request_handler(request):
    conn = sqlite3.connect(visits_db)  #connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  #make cursor into database (allows us to execute commands)

    c.execute('''CREATE TABLE IF NOT EXISTS esp_instructions (request_id integer primary key AUTOINCREMENT, esp_id int, sensor text, instruction text, result text, request_time timestamp, completed int, completed_time timestamp);''')

    if request['method'] == "POST":
        try:
            update = request['form']['update']
            if update == "T": #updating an existing row in databse
                result = request['form']['result']
                request_num = int(request['form']['request_id'])
                c.execute('''UPDATE esp_instructions SET result = ?, completed = ?, completed_time = ? WHERE request_id = ? ;''', (result, 1, datetime.datetime.now(), request_num))
                conn.commit() #commit commands
                conn.close() #close connection to database
                return  "db updated"
            else: #inserting a new row into databse
                esp = int(request['form']['esp'])
                sensor = request['form']['sensor']
                instruction = request['form']['instruction']

                #begin error checking

                check = instruction.split("$")
                valid = True
                error = ""
                if sensor == "IMU": #checking all possible requests for the IMU
                    if check[0] == "accel" or check[0] == "gyro":
                        if check[1] == "single":
                            if check[2] not in ["x","y","z","all"]:
                                valid = False
                                error = "invalid params (IMU) ~ please specify an axis"
                        elif check[1] == "avg":
                            if "," not in check[2]:
                                valid = False
                                error = "invalid params (IMU - accel/gyro - avg) ~ please specify a time interval"
                            elif check[2].split(",")[0] not in ["x","y","z","all"]:
                                valid = False
                                error = "invalid params (IMU) ~ please specify an axis"
                        else:
                            valid = False
                            error = "invalid request (IMU - accel/gyro) ~ request single or average recording"
                    elif check[0] == "temp" or check[0] == "compass":
                        if check[1] not in ["single","avg"]:
                            valid = False
                            error = "invalid request (IMU - temp/compass) ~ request single or average recording"
                        elif check[1] == "avg":
                            if "," not in check[2]:
                                valid = False
                                error = "invalid params (IMU - temp/compass - avg) ~ please specify a time interval"
                    else:
                        valid = False
                        error = "invalid category (IMU)"
                elif sensor == "MIC": #checking all possible requests for the microphone
                    if check[0] == "rec":
                        if int(check[1]) > 5:
                            valid = False
                            error = "invalid params (MIC) ~ can't record over 5 seconds"
                    else:
                        valid = False
                        error = "invalid request (MIC) ~ request a recording for some time interval"
                elif sensor == "GPS": #checking all possible requests for the GPS
                    if check[0] not in ["status", "loc", "time"]:
                        valid = False
                        error = "invalid category (GPS)"
                    elif check[0] == "status":
                        if check[1] not in ["con", "sat"]:
                            valid = False
                            error = "invlaid request (GPS - status) ~ request check connection or number of satellites"
                elif sensor == "USS": #checking all possible requests for the ultrasonic senosr
                    if check[0] not in ["single", "avg"]:
                        valid = False
                        error = "invalid request (USS) ~ request single or average recording"
                    elif check[0] == "avg":
                        if len(check) < 2:
                            valid = False
                            error = "invalid params (USS - avg) ~ please specify a time interval"
                elif sensor == "LED": #checking all possible requests for the LED
                    if check[0] == "set":
                        if len(check) < 2 or check[1] not in ["0","1"]:
                            valid = False
                            error = "invalid request (LED - set) ~ request on or off"
                    elif check[0] == "flash":
                        if len(check) < 2 or check[1] not in ["0","1"]:
                            valid = False
                            error = "invalid request (LED - flash) ~ request on or off"
                    else:
                        valid = False
                        error = "invalid category (LED) ~ request either set or flash"
                elif sensor == "BAT": #checking all possible requests for the battery
                    if not check[0] == "volt":
                        valid = False
                        error = "invalid request (BAT) ~ request to check voltage"
                elif sensor == "OFF": #checking all possible requests for the OFF
                    if not check[0] == "sleep":
                        valid = False
                        error = "invalid request (OFF) ~ request sleep mode"
                else: #did not specify one of these valid sensors
                    valid = False
                    error = "invalid sensor ~ valid sensors are [IMU, MIC, GPS, USS, LED, BAT, OFF]"

                if not valid:
                    conn.close()
                    return error

                #end error checking

                c.execute('''INSERT into esp_instructions VALUES (?,?,?,?,?,?,?,?);''',(None, esp, sensor, instruction, '', datetime.datetime.now(), 0, None))
                conn.commit() #commit commands
                conn.close() #close connection to database
                return "command added to db"
        except:
            return "bad inputs to POST request"

    if request['method' ] == "GET":
        if 'esp_id' in request['args']: #want only incomplete tasks for a specific esp; request coming from ESP
            esp = int(request['values']['esp_id'])
            result = c.execute('''SELECT * FROM esp_instructions WHERE esp_id = ? AND completed = 0 ORDER BY request_time ASC;''', (esp,)).fetchone()
            conn.commit()
            conn.close()
            request_id = result[0]
            sensor = result[2]
            instruction = result[3]
            return str(request_id) + '~' + str(sensor) + '~' + str(instruction)
        else: #want all tasks for all esps, only complete; request coming from website
            things = c.execute('''SELECT * FROM esp_instructions;''',).fetchall()
            outs = ""
            for x in things:
                outs+=str(x)+"\n"
            return outs
            # result = c.execute('''SELECT * FROM esp_instructions WHERE completed = 1;''')
            # result_list = []
            # for req in result:
            #     result_list.append({'id': req[0], 'time': req[5], 'result': req[4]})
            # conn.commit()  # commit commands
            # conn.close()  # close connection to database
            # return json.dumps(result_list)

    else:
        conn.commit()  # commit commands
        conn.close()  # close connection to database
        return "bad request"
