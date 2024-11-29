#!/usr/bin/env php
<?php
// Output HTTP headers (ensure correct order)
header("Content-Type: text/html");

// Output HTML content
echo <<<HTML
<html>
<head>
<title>Hello World - First CGI Program</title>
</head>
<body>
<h2>Hello World! This is my first CGI program</h2>
</body>
</html>
HTML;
?>
