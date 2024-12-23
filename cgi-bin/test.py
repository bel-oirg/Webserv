#!/usr/bin/env python3
# test.py - Place this in your cgi-bin directory
import cgi
import cgitb
import html
import os

# Enable CGI traceback for debugging
cgitb.enable()

# Print HTTP headers
print("Content-Type: text/html")
print()  # Blank line needed after headers

# Start HTML output
print("""<!DOCTYPE html>
<html>
<head>
    <title>Python CGI Test</title>
</head>
<body>
    <h1>Python CGI Test Page</h1>
""")

# Get form data if any
form = cgi.FieldStorage()
if form.getvalue('name'):
    print(f"<p>Hello, {html.escape(form.getvalue('name'))}!</p>")

# Display environment variables
print("<h2>Environment Variables:</h2>")
print("<pre>")
for key, value in os.environ.items():
    print(f"{html.escape(key)}: {html.escape(value)}")
print("</pre>")

# Display a test form
print("""
    <h2>Test Form</h2>
    <form method="post" action="test.py">
        <label for="name">Enter your name:</label><br>
        <input type="text" id="name" name="name"><br>
        <input type="submit" value="Submit">
    </form>
</body>
</html>
""")