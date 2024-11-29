#!/bin/bash

# Output HTTP headers
echo "Content-Type: text/html"
echo ""

# Output HTML content
cat <<EOF
<html>
<head>
<title>Hello World - First CGI Program</title>
</head>
<body>
<h2>Hello World! This is my first CGI program</h2>
</body>
</html>
EOF
