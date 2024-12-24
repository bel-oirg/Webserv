<?php
// Set the HTTP header to specify content type (HTML)
header('Content-Type: text/html');

// Get the request method (GET or POST)
$request_method = $_SERVER['REQUEST_METHOD'];

// If it's a POST request, read the body from php://input and search for the name
if ($request_method == 'POST') {
    // Read the POST body from php://input (similar to sys.stdin.read() in Python)
    $post_body = file_get_contents('php://input');
    $name = null;

    // Parse the POST data for 'name' (this is equivalent to parsing query strings in PHP)
    parse_str($post_body, $data);  // Convert the POST data to an associative array

    // Look for 'name' in the parsed data
    if (isset($data['name'])) {
        $name = $data['name'];
    }

    // If a name is found, print a greeting
    if ($name) {
        // Decode the name (replace %20 with space, if URL-encoded)
        $name = urldecode($name);  // Automatically decode URL-encoded strings
        echo "<h1>Hello, " . htmlspecialchars($name) . "!</h1>";  // Output the greeting
    } else {
        echo "<h1>Sorry, 'name' not found in the POST data.</h1>";
    }

} elseif ($request_method == 'GET') {
    // If it's a GET request, display a form to enter the name
    echo '<h1>Enter your name</h1>';
    echo '<form action="/cgi-bin/name.php" method="POST">';
    echo '<label for="name">Name: </label>';
    echo '<input type="text" id="name" name="name">';
    echo '<button type="submit">Submit</button>';
    echo '</form>';

} else {
    echo "<h1>Unsupported Request Method</h1>";
}
?>
