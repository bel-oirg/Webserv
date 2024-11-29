#!/usr/bin/env python3

# Import required modules
import cgi
import cgitb

# Enable debugging (helps to see errors in the browser)
cgitb.enable()

# Output HTTP headers (ensure there are no extra spaces or characters)
print("Content-Type: text/html\n")

# Output the HTML body
print("""
<html>
<head>
<title>Hello World - First CGI Program</title>
</head>
<body>
<h2>Hello World! This is my first CGI program</h2>
</body>
</html>
""")
