#!/usr/bin/env php
<?php
header('Content-Type: text/html; charset=utf-8');

// Helper function to format bytes
function formatBytes($bytes, $precision = 2) {
    $units = ['B', 'KB', 'MB', 'GB', 'TB'];
    $bytes = max($bytes, 0);
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
    $pow = min($pow, count($units) - 1);
    return round($bytes / (1024 ** $pow), $precision) . ' ' . $units[$pow];
}

// Function to test file permissions
function testWritePermissions($path) {
    try {
        $testFile = $path . "/cgi_test_" . time() . ".txt";
        $success = @file_put_contents($testFile, "Test");
        if ($success !== false) {
            unlink($testFile);
            return true;
        }
        return false;
    } catch (Exception $e) {
        return false;
    }
}

// Function to test database connection
function testDatabaseConnection() {
    $databases = [
        'MySQL' => function() {
            return @mysqli_connect('localhost', 'root', '');
        },
        'PostgreSQL' => function() {
            return @pg_connect("host=localhost user=postgres password=postgres");
        },
        'SQLite' => function() {
            try {
                $db = new PDO('sqlite::memory:');
                return $db;
            } catch (Exception $e) {
                return false;
            }
        }
    ];

    $results = [];
    foreach ($databases as $name => $test) {
        $results[$name] = $test() !== false ? "Available" : "Not Available/Not Configured";
    }
    return $results;
}

// Function to test network connectivity
function testNetworkConnectivity() {
    $hosts = ['google.com', 'github.com', 'stackoverflow.com'];
    $results = [];
    foreach ($hosts as $host) {
        $start = microtime(true);
        $fp = @fsockopen($host, 80, $errno, $errstr, 5);
        $end = microtime(true);
        if ($fp) {
            $results[$host] = round(($end - $start) * 1000, 2) . "ms";
            fclose($fp);
        } else {
            $results[$host] = "Failed";
        }
    }
    return $results;
}

// Test image processing capabilities
function testImageProcessing() {
    $capabilities = [];
    if (extension_loaded('gd')) {
        $info = gd_info();
        $capabilities['GD Version'] = $info['GD Version'];
        $capabilities['FreeType Support'] = $info['FreeType Support'] ? "Yes" : "No";
        $capabilities['JPG Support'] = $info['JPEG Support'] ? "Yes" : "No";
        $capabilities['PNG Support'] = $info['PNG Support'] ? "Yes" : "No";
        $capabilities['GIF Support'] = $info['GIF Read Support'] ? "Yes" : "No";
    } else {
        $capabilities['GD'] = "Not Available";
    }
    return $capabilities;
}

?>
<!DOCTYPE html>
<html>
<head>
    <title>Comprehensive PHP CGI Test Suite</title>
    <style>
        :root {
            --primary-color: #2c3e50;
            --secondary-color: #34495e;
            --accent-color: #3498db;
            --success-color: #2ecc71;
            --warning-color: #f1c40f;
            --danger-color: #e74c3c;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: #f5f6fa;
            color: var(--primary-color);
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        .header {
            background: var(--primary-color);
            color: white;
            padding: 20px;
            border-radius: 10px;
            margin-bottom: 20px;
        }
        .section {
            background: white;
            padding: 20px;
            margin-bottom: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        h1, h2, h3 {
            margin-top: 0;
        }
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
        }
        .status {
            display: inline-block;
            padding: 5px 10px;
            border-radius: 5px;
            font-size: 14px;
        }
        .status-success { background: var(--success-color); color: white; }
        .status-warning { background: var(--warning-color); color: black; }
        .status-danger { background: var(--danger-color); color: white; }
        pre {
            background: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            overflow-x: auto;
        }
        table {
            width: 100%;
            border-collapse: collapse;
        }
        th, td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background: var(--secondary-color);
            color: white;
        }
        form {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 5px;
        }
        input[type="text"], textarea {
            width: 100%;
            padding: 8px;
            margin: 5px 0;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        input[type="submit"] {
            background: var(--accent-color);
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background: #2980b9;
        }
        .tab-container {
            border: 1px solid #ddd;
            border-radius: 5px;
            overflow: hidden;
        }
        .tab-header {
            background: var(--secondary-color);
            padding: 10px;
            color: white;
        }
        .metric {
            display: flex;
            justify-content: space-between;
            padding: 10px;
            border-bottom: 1px solid #ddd;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Comprehensive PHP CGI Test Suite</h1>
            <p>Server Time: <?php echo date('Y-m-d H:i:s'); ?></p>
        </div>

        <div class="section">
            <h2>System Information</h2>
            <div class="grid">
                <div class="tab-container">
                    <div class="tab-header">PHP Configuration</div>
                    <div class="metric">
                        <span>PHP Version</span>
                        <span><?php echo phpversion(); ?></span>
                    </div>
                    <div class="metric">
                        <span>Server API</span>
                        <span><?php echo php_sapi_name(); ?></span>
                    </div>
                    <div class="metric">
                        <span>Memory Limit</span>
                        <span><?php echo ini_get('memory_limit'); ?></span>
                    </div>
                    <div class="metric">
                        <span>Max Execution Time</span>
                        <span><?php echo ini_get('max_execution_time'); ?>s</span>
                    </div>
                </div>

                <div class="tab-container">
                    <div class="tab-header">Server Information</div>
                    <div class="metric">
                        <span>Server Software</span>
                        <span><?php echo $_SERVER['SERVER_SOFTWARE'] ?? 'Not Available'; ?></span>
                    </div>
                    <div class="metric">
                        <span>Server Protocol</span>
                        <span><?php echo $_SERVER['SERVER_PROTOCOL'] ?? 'Not Available'; ?></span>
                    </div>
                    <div class="metric">
                        <span>Document Root</span>
                        <span><?php echo $_SERVER['DOCUMENT_ROOT'] ?? 'Not Available'; ?></span>
                    </div>
                </div>
            </div>
        </div>

        <div class="section">
            <h2>CGI Environment Test</h2>
            <pre>
<?php
foreach ($_SERVER as $key => $value) {
    echo htmlspecialchars("$key: $value\n");
}
?>
            </pre>
        </div>

        <div class="section">
            <h2>File System Tests</h2>
            <table>
                <tr>
                    <th>Test</th>
                    <th>Result</th>
                </tr>
                <tr>
                    <td>Document Root Write Permission</td>
                    <td>
                        <?php 
                        $writeTest = testWritePermissions($_SERVER['DOCUMENT_ROOT']);
                        $class = $writeTest ? 'status-success' : 'status-danger';
                        echo "<span class='status $class'>" . ($writeTest ? 'Writable' : 'Not Writable') . "</span>";
                        ?>
                    </td>
                </tr>
                <tr>
                    <td>Temporary Directory Write Permission</td>
                    <td>
                        <?php 
                        $tempTest = testWritePermissions(sys_get_temp_dir());
                        $class = $tempTest ? 'status-success' : 'status-danger';
                        echo "<span class='status $class'>" . ($tempTest ? 'Writable' : 'Not Writable') . "</span>";
                        ?>
                    </td>
                </tr>
                <tr>
                    <td>Upload Max Filesize</td>
                    <td><?php echo ini_get('upload_max_filesize'); ?></td>
                </tr>
                <tr>
                    <td>Post Max Size</td>
                    <td><?php echo ini_get('post_max_size'); ?></td>
                </tr>
            </table>
        </div>

        <div class="section">
            <h2>Database Connectivity Tests</h2>
            <table>
                <tr>
                    <th>Database</th>
                    <th>Status</th>
                </tr>
                <?php
                $dbTests = testDatabaseConnection();
                foreach ($dbTests as $db => $status) {
                    $class = $status === 'Available' ? 'status-success' : 'status-warning';
                    echo "<tr><td>$db</td><td><span class='status $class'>$status</span></td></tr>";
                }
                ?>
            </table>
        </div>

        <div class="section">
            <h2>Network Connectivity Tests</h2>
            <table>
                <tr>
                    <th>Host</th>
                    <th>Response Time</th>
                </tr>
                <?php
                $networkTests = testNetworkConnectivity();
                foreach ($networkTests as $host => $result) {
                    $class = $result !== 'Failed' ? 'status-success' : 'status-danger';
                    echo "<tr><td>$host</td><td><span class='status $class'>$result</span></td></tr>";
                }
                ?>
            </table>
        </div>

        <div class="section">
            <h2>Image Processing Capabilities</h2>
            <table>
                <tr>
                    <th>Feature</th>
                    <th>Status</th>
                </tr>
                <?php
                $imageTests = testImageProcessing();
                foreach ($imageTests as $feature => $status) {
                    $class = $status !== 'Not Available' ? 'status-success' : 'status-warning';
                    echo "<tr><td>$feature</td><td><span class='status $class'>$status</span></td></tr>";
                }
                ?>
            </table>
        </div>

        <div class="section">
            <h2>Form Tests</h2>
            <div class="grid">
                <div>
                    <h3>POST Test</h3>
                    <form method="post" enctype="multipart/form-data">
                        <p><input type="text" name="post_test" placeholder="Enter test text"></p>
                        <p><textarea name="post_area" placeholder="Enter multiple lines of text"></textarea></p>
                        <p><input type="file" name="test_file"></p>
                        <p><input type="submit" value="Submit POST Test"></p>
                    </form>

                    <?php if (!empty($_POST)): ?>
                        <h4>POST Data Received:</h4>
                        <pre><?php print_r($_POST); ?></pre>
                    <?php endif; ?>

                    <?php if (!empty($_FILES)): ?>
                        <h4>Files Received:</h4>
                        <pre><?php print_r($_FILES); ?></pre>
                    <?php endif; ?>
                </div>

                <div>
                    <h3>GET Test</h3>
                    <form method="get">
                        <p><input type="text" name="get_test" placeholder="Enter test text"></p>
                        <p><input type="submit" value="Submit GET Test"></p>
                    </form>

                    <?php if (!empty($_GET)): ?>
                        <h4>GET Data Received:</h4>
                        <pre><?php print_r($_GET); ?></pre>
                    <?php endif; ?>
                </div>
            </div>
        </div>

        <div class="section">
            <h2>PHP Extensions</h2>
            <pre><?php echo implode("\n", get_loaded_extensions()); ?></pre>
        </div>

        <div class="section">
            <h2>PHP INI Settings</h2>
            <pre><?php print_r(ini_get_all()); ?></pre>
        </div>
    </div>
</body>
</html>