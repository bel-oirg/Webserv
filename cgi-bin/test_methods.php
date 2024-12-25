#!/usr/bin/php-cgi
<?php
header('Content-Type: text/html');

// Function to get request body
function getRequestBody() {
    return file_get_contents('php://input');
}

// Function to get sanitized request data
function getRequestData() {
    $data = [];
    if ($_SERVER['REQUEST_METHOD'] === 'GET') {
        $data = $_GET;
    } elseif ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $data = $_POST;
        if (empty($data)) {
            $raw = getRequestBody();
            if ($raw) {
                $data['raw_body'] = $raw;
            }
        }
    } else {
        $raw = getRequestBody();
        if ($raw) {
            $data['raw_body'] = $raw;
        }
    }
    return $data;
}

$method = $_SERVER['REQUEST_METHOD'];
$requestData = getRequestData();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>HTTP Methods Test Page</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .method-info { margin: 20px 0; padding: 10px; border: 1px solid #ccc; }
        .current { background-color: #e6ffe6; }
        form { margin: 10px 0; }
        button { margin: 5px; padding: 5px 10px; }
    </style>
</head>
<body>
    <h1>HTTP Methods Test Page</h1>
    
    <div class="method-info <?php echo $method === 'GET' ? 'current' : ''; ?>">
        <h2>GET Test</h2>
        <form method="GET">
            <input type="text" name="test_data" placeholder="Enter test data">
            <button type="submit">Send GET Request</button>
        </form>
    </div>

    <div class="method-info <?php echo $method === 'POST' ? 'current' : ''; ?>">
        <h2>POST Test</h2>
        <form method="POST">
            <input type="text" name="test_data" placeholder="Enter test data">
            <button type="submit">Send POST Request</button>
        </form>
    </div>

    <div class="method-info <?php echo $method === 'DELETE' ? 'current' : ''; ?>">
        <h2>DELETE Test</h2>
        <button onclick="sendDelete()">Send DELETE Request</button>
    </div>

    <div class="method-info">
        <h2>Current Request Information:</h2>
        <pre>
Method: <?php echo htmlspecialchars($method); ?>

Request Data:
<?php print_r(htmlspecialchars(print_r($requestData, true))); ?>

Headers:
<?php print_r(htmlspecialchars(print_r(getallheaders(), true))); ?>
        </pre>
    </div>

    <script>
        function sendDelete() {
            fetch(window.location.href, {
                method: 'DELETE',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({test: 'delete_test'})
            })
            .then(response => response.text())
            .then(data => {
                window.location.reload();
            })
            .catch(error => {
                console.error('Error:', error);
                alert('Error sending DELETE request');
            });
        }
    </script>
</body>
</html>