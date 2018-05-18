import sqlite3
import datetime

example_db = "esp_instructions.db" # just come up with name of database
conn = sqlite3.connect(example_db)  # connect to that database (will create if it doesn't already exist)
c = conn.cursor()  # make cursor into database (allows us to execute commands)
c.execute('''CREATE TABLE IF NOT EXISTS esp_instructions (request_id integer primary key AUTOINCREMENT, esp_id int, sensor text, instruction text, result text, request_time timestamp, completed int, completed_time timestamp);''')
conn.commit() # commit commands
conn.close() # close connection to database
