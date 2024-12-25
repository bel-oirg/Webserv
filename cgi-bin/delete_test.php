<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Delete Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        form { margin-bottom: 20px; }
        .response { margin-top: 20px; border: 1px solid #ccc; padding: 10px; }
    </style>
</head>
<body>
    <h1>File Delete Test</h1>

    <!-- Form to specify the file path -->
    <form id="postForm">
        <label for="filePath">Enter File Path:</label><br>
        <input type="text" id="filePath" name="filePath" placeholder="/path/to/your/file.txt" required style="width: 100%;"><br><br>
        <button type="submit">Submit File Path</button>
    </form>

    <!-- Button to trigger file deletion -->
    <button id="deleteButton">Delete File</button>

    <!-- Response display -->
    <div class="response" id="response" hidden>
        <h3>Response</h3>
        <pre id="responseText"></pre>
    </div>

    <script>
        const postForm = document.getElementById('postForm');
        const deleteButton = document.getElementById('deleteButton');
        const responseDiv = document.getElementById('response');
        const responseText = document.getElementById('responseText');

        // Helper function to display response
        function showResponse(data) {
            responseDiv.hidden = false;
            responseText.textContent = JSON.stringify(data, null, 2);
        }

        // Handle POST request to send file path
        postForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            const filePath = document.getElementById('filePath').value;

            try {
                const response = await fetch('/delete_test.php', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ file_path: filePath })
                });
                const data = await response.json();
                showResponse(data);
            } catch (error) {
                showResponse({ status: 'error', message: error.message });
            }
        });

        // Handle DELETE request to delete the file
        deleteButton.addEventListener('click', async () => {
            try {
                const response = await fetch('/delete_test.php', {
                    method: 'DELETE'
                });
                const data = await response.json();
                showResponse(data);
            } catch (error) {
                showResponse({ status: 'error', message: error.message });
            }
        });
    </script>
</body>
</html>
