#!/usr/bin/env ruby

# Output HTTP headers
puts "Content-Type: text/html\n\n"

# Output HTML content
puts <<~HTML
<html>
<head>
<title>Hello World - First CGI Program</title>
</head>
<body>
<h2>Hello World! This is my first CGI program</h2>
</body>
</html>
HTML
