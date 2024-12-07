#!/usr/bin/php
<?php
// Set the content type to HTML
header('Content-Type: text/html');

// Function to handle the form display
function showForm($num1 = '', $num2 = '', $operation = '', $result = '', $error = '') {
    echo <<<HTML
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>PHP CGI Calculator</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                background-color: #f4f4f9;
                margin: 0;
                padding: 0;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
            }
            .calculator {
                background-color: white;
                padding: 30px;
                border-radius: 8px;
                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
                width: 320px;
            }
            h1 {
                color: #4CAF50;
                text-align: center;
            }
            input[type="number"], select, input[type="submit"] {
                width: 100%;
                padding: 12px;
                margin: 10px 0;
                border: 1px solid #ddd;
                border-radius: 5px;
                font-size: 16px;
            }
            input[type="submit"] {
                background-color: #4CAF50;
                color: white;
                border: none;
                cursor: pointer;
            }
            input[type="submit"]:hover {
                background-color: #45a049;
            }
            .result, .error {
                text-align: center;
                font-size: 18px;
                margin-top: 20px;
            }
            .error {
                color: red;
            }
            .result {
                color: #333;
                font-weight: bold;
            }
        </style>
    </head>
    <body>
    <div class="calculator">
        <h1>PHP CGI Calculator</h1>
        <form method="POST">
            <input type="number" name="num1" value="$num1" placeholder="Enter first number" required>
            <input type="number" name="num2" value="$num2" placeholder="Enter second number" required>
            <select name="operation" required>
                <option value="add" $operation === 'add' ? 'selected' : ''>Addition (+)</option>
                <option value="subtract" $operation === 'subtract' ? 'selected' : ''>Subtraction (-)</option>
                <option value="multiply" $operation === 'multiply' ? 'selected' : ''>Multiplication (×)</option>
                <option value="divide" $operation === 'divide' ? 'selected' : ''>Division (÷)</option>
            </select>
            <input type="submit" value="Calculate">
        </form>
HTML;

    if ($error) {
        echo "<div class='error'>$error</div>";
    }

    if ($result !== '') {
        echo "<div class='result'>Result: $result</div>";
    }

    echo "</div></body></html>";
}

// Initialize variables
$result = '';
$error = '';

// Check if the form has been submitted
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    // Get form values
    $num1 = isset($_POST['num1']) ? $_POST['num1'] : '';
    $num2 = isset($_POST['num2']) ? $_POST['num2'] : '';
    $operation = isset($_POST['operation']) ? $_POST['operation'] : '';

    // Validate the numbers and perform calculation
    if (is_numeric($num1) && is_numeric($num2)) {
        switch ($operation) {
            case 'add':
                $result = $num1 + $num2;
                break;
            case 'subtract':
                $result = $num1 - $num2;
                break;
            case 'multiply':
                $result = $num1 * $num2;
                break;
            case 'divide':
                if ($num2 != 0) {
                    $result = $num1 / $num2;
                } else {
                    $error = "Cannot divide by zero!";
                }
                break;
            default:
                $error = "Invalid operation!";
        }
    } else {
        $error = "Please enter valid numbers!";
    }
} else {
    // Display the form if no submission yet
    $num1 = $num2 = $operation = '';
    showForm($num1, $num2, $operation, '', '');
    exit; // Exit early to prevent showing empty results when the form is not yet submitted.
}

// Show the form with results
showForm($num1, $num2, $operation, $result, $error);
?>
