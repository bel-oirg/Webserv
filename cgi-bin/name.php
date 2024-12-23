<?php
// Check if the form was submitted via POST
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Access the 'name' field from the POST data
    $name = htmlspecialchars($_POST['name']);
    echo "<h1>Hello, $name!</h1>"; // Display the entered name
} else {
    // If the form wasn't submitted, show the form
    echo '<h1>Enter your name</h1>';
    echo '<form action="name.php" method="POST">';
    echo '<label for="name">Name: </label>';
    echo '<input type="text" id="name" name="name">';
    echo '<button type="submit">Submit</button>';
    echo '</form>';
}
?>
