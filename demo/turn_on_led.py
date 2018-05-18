import datetime
def request_handler(request):
    if request['method'] == "POST":
        return 'POST requests not allowed'
    elif request['method'] == "GET":
        if 'esp_id' not in request['args']:
            return 'Include an esp_id'
        if request['values']['esp_id'] == '2':
            if datetime.datetime.now().second % 15 == 0:
                if datetime.datetime.now().second % 2 == 0:
                    return '123~LED~flash$1$3'
                else:
                    return '123~LED~set$0'