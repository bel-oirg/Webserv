#!/usr/bin/php-cgi
<?php
// Enable error reporting for debugging
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Output HTTP headers
header("Content-Type: text/html");

// HTML content
echo "<html><head><title>PHP CGI Test</title></head>";
echo "<body>";
echo "<h1>PHP CGI Script Test</h1>";
echo "<p>This is a simple test to check if PHP CGI is working.</p>";

// Display received form data (if any)
if (!empty($_GET) || !empty($_POST)) {
    echo "<h2>Received Form Data:</h2>";
    echo "<ul>";
    foreach ($_REQUEST as $key => $value) {
        echo "<li><b>$key:</b> $value</li>";
    }
    echo "</ul>";
} else {
    echo "<p>No form data received.</p>";
}

echo "</body></html>";
?>
