#!/usr/bin/python3
import os

print("Content-Type: text/plain")
print()

if os.environ.get('HTTP_COOKIE'):
    print(f"The Value of Cookie: {os.environ.get('HTTP_COOKIE')}")
else:
    print("No Cookie found")

#curl --cookie "test_cookie=my_value" "http://127.0.0.1:8080/cgi-bin/check_cookie.py?key=test_cookie"