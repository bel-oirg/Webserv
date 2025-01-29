#!/usr/bin/php-cgi
<?php
if (!isset($_COOKIE["username"])) {
	setcookie("username", "Hamza", time() + 30); // Cookie expires in 30s
}
?>

<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<meta viewport="width=device-width, initial-scale=1">
		<title>Cookie</title>
	</head>
	<body>
		<h1>Welcome back, <?php echo (isset($_COOKIE["username"]) ? $_COOKIE["username"] : "guest") ?></h1>
	</body>
</html>