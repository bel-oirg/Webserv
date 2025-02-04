
#!/usr/bin/env python3
import time

print("Content-Type: text/html\n")

# time.sleep(5)


print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Galaxy Animation</title>
    <style>
        body {
            margin: 0;
            background: radial-gradient(circle at center, #000428, #004e92);
            overflow: hidden;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }
    </style>
</head>
<body>
    <svg width="800" height="800" xmlns="http://www.w3.org/2000/svg">
        <!-- Central Star -->
        <circle cx="400" cy="400" r="20" fill="yellow">
            <animateTransform attributeName="transform" type="rotate" from="0 400 400" to="360 400 400" dur="20s" repeatCount="indefinite" />
        </circle>
        
        <!-- Orbiting Stars -->
        <circle cx="500" cy="400" r="5" fill="white">
            <animateTransform attributeName="transform" type="rotate" from="0 400 400" to="360 400 400" dur="5s" repeatCount="indefinite" />
        </circle>
        <circle cx="600" cy="400" r="8" fill="#FF5733">
            <animateTransform attributeName="transform" type="rotate" from="0 400 400" to="360 400 400" dur="8s" repeatCount="indefinite" />
        </circle>
        <circle cx="700" cy="400" r="4" fill="#00FF00">
            <animateTransform attributeName="transform" type="rotate" from="0 400 400" to="360 400 400" dur="12s" repeatCount="indefinite" />
        </circle>

        <!-- Twinkling Stars -->
        <circle cx="150" cy="100" r="3" fill="white">
            <animate attributeName="r" values="2;4;2" dur="2s" repeatCount="indefinite" />
        </circle>
        <circle cx="650" cy="200" r="4" fill="white">
            <animate attributeName="r" values="3;5;3" dur="3s" repeatCount="indefinite" />
        </circle>
        <circle cx="200" cy="700" r="2" fill="white">
            <animate attributeName="r" values="2;4;2" dur="1.5s" repeatCount="indefinite" />
        </circle>
    </svg>
</body>
</html>
""")
