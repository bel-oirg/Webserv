#!/usr/bin/env php
<?php
header('Content-Type: text/html; charset=utf-8');
?>
<!DOCTYPE html>
<html>
<head>
    <title>PHP CGI Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }
        .section { background: #f5f5f5; padding: 20px; margin-bottom: 20px; border-radius: 5px; }
        h2 { color: #333; margin-top: 0; }
        .key { font-weight: bold; color: #444; }
        pre { background: #fff; padding: 10px; border-radius: 3px; }
    </style>
</head>
<body>
    <h1>PHP CGI Environment Test</h1>

    <div class="section">
        <h2>PHP Information</h2>
        <p><span class="key">PHP Version:</span> <?php echo phpversion(); ?></p>
        <p><span class="key">Server Software:</span> <?php echo $_SERVER['SERVER_SOFTWARE'] ?? 'Not Available'; ?></p>
        <p><span class="key">Server API:</span> <?php echo php_sapi_name(); ?></p>
        <p><span class="key">Current Time:</span> <?php echo date('Y-m-d H:i:s'); ?></p>
    </div>

    <div class="section">
        <h2>CGI Environment Variables</h2>
        <pre>
<?php
foreach ($_SERVER as $key => $value) {
    echo htmlspecialchars("$key: $value\n");
}
?>
        </pre>
    </div>

    <div class="section">
        <h2>Form Data Received</h2>
        <?php if (!empty($_POST)): ?>
            <pre>
<?php
foreach ($_POST as $key => $value) {
    echo htmlspecialchars("$key: $value\n");
}
?>
            </pre>
        <?php else: ?>
            <p>No POST data received</p>
        <?php endif; ?>

        <?php if (!empty($_GET)): ?>
            <pre>
<?php
foreach ($_GET as $key => $value) {
    echo htmlspecialchars("$key: $value\n");
}
?>
            </pre>
        <?php else: ?>
            <p>No GET data received</p>
        <?php endif; ?>
    </div>

    <div class="section">
        <h2>Test Forms</h2>
        <h3>POST Test</h3>
        <form method="post">
            <p><input type="text" name="test_text" placeholder="Enter test text"></p>
            <p><input type="submit" value="Submit POST Test"></p>
        </form>

        <h3>GET Test</h3>
        <form method="get">
            <p><input type="text" name="query" placeholder="Enter test query"></p>
            <p><input type="submit" value="Submit GET Test"></p>
        </form>
    </div>

    <div class="section">
        <h2>PHP Extensions</h2>
        <pre>
<?php echo implode("\n", get_loaded_extensions()); ?>
        </pre>
    </div>
</body>
</html>