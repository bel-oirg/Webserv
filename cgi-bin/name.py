#!/usr/bin/env python3

import sys
import os

# Set the HTTP header to specify content type (HTML)

# Get the request method (GET or POST)
request_method = os.environ['REQUEST_METHOD']

# If it's a POST request, read the body from stdin and search for the name
if request_method == 'POST':
    # Read the POST body from stdin (this is like PHP's file_get_contents('php://input'))
    post_body = sys.stdin.read()
    name = None

    # Parse the POST data for 'name'
    for line in post_body.split('&'):
        key_value = line.split('=')
        if key_value[0] == 'name':  # Look for the 'name' key in the POST data
            name = key_value[1]
            break

    # If a name is found, print a greeting
    if name:
        # Decode the name (replace %20 with space, if URL-encoded)
        name = name.replace('%20', ' ')  # Manually replace URL encoding for space (%20)

        print(f"<h1>Hello, {name}!</h1>")
    else:
        print("<h1>Sorry, 'name' not found in the POST data.</h1>")

# If it's a GET request, display a form to enter the name
elif request_method == 'GET':
    print('<h1>Enter your name</h1>')
    print('<form action="/cgi-bin/name.py" method="POST">')
    print('<label for="name">Name: </label>')
    print('<input type="text" id="name" name="name">')
    print('<button type="submit">Submit</button>')
    print('</form>')

else:
    print("<h1>Unsupported Request Method</h1>")
