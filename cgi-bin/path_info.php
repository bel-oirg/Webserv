<?php
// Set the content type to plain text
header("Content-Type: text/plain");

// Get the PATH_INFO environment variable
$path_info = $_SERVER['PATH_INFO'] ?? 'PATH_INFO is not set';

// Display the PATH_INFO value
echo "PATH_INFO: " . $path_info . "\n\n";

// Example of using PATH_INFO for dynamic routing
if ($path_info !== 'PATH_INFO is not set') {
    // Split the PATH_INFO into segments
    $segments = explode('/', trim($path_info, '/'));

    // Display the segments
    echo "Path Segments:\n";
    foreach ($segments as $index => $segment) {
        echo "Segment $index: $segment\n";
    }

    // Example: Handle specific routes
    if ($segments[0] === 'users') {
        echo "\nYou are accessing the 'users' section.\n";
        if (isset($segments[1])) {
            echo "User ID: " . $segments[1] . "\n";
        }
    } elseif ($segments[0] === 'products') {
        echo "\nYou are accessing the 'products' section.\n";
        if (isset($segments[1])) {
            echo "Product ID: " . $segments[1] . "\n";
        }
    }
} else {
    echo "No additional path information provided.\n";
}
?>