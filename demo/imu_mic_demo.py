import datetime
def request_handler(request):
    if request['method'] == "POST":
        return 'POST requests not allowed'
    elif request['method'] == "GET":
        if 'esp_id' not in request['args']:
            return 'Include an esp_id'
        if request['values']['esp_id'] == '2':
            sec = datetime.datetime.now().second
            sec = sec % 60
            if sec % 15 == 0:
                if sec == 0:
                    return '123~IMU~accel$avg$z,1'
                elif sec == 15:
                    return '123~IMU~gyro$single$z'
                elif sec == 30:
                    return '123~IMU~temp$avg$3'
                else:
                    return '123~MIC~rec$1'