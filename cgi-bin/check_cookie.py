#!/usr/bin/python3
import os

print("Content-Type: text/plain")
print()

if os.environ.get('HTTP_COOKIE'):
    print(f"The Value of Cookie: {os.environ.get('HTTP_COOKIE')}")
else:
    print("No Cookie found")
