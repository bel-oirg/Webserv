#!/usr/bin/env python3
import cgi
import csv
import os

# Set the header for the response to be HTML
print("Content-type: text/html\n")

# HTML form for user input (optional for other queries)
print("""
<html>
    <head><title>Search Login Level</title></head>
    <body>
        <h2>Search for a User Login Level</h2>
        <form method="GET">
            Login: <input type="text" name="login" />
            <input type="submit" value="Search" />
        </form>
""")

# Retrieve the path info from the URL (the directory part)
path_info = os.environ.get('PATH_INFO', '').strip('/')

# Retrieve the login query parameter from the URL
form = cgi.FieldStorage()
login_search = form.getvalue('login', '')

# Ensure that both path_info (directory) and login_search (login) are provided
if path_info and login_search:
    # Construct the path to the CSV file using the directory from PATH_INFO
    csv_file_path = f"/Users/bel-oirg/Desktop/web_/{path_info}/data.csv"
    
    # Function to search for the login in the CSV file
    def get_level_from_csv(file_path, login):
        try:
            with open(file_path, mode='r') as file:
                reader = csv.reader(file)
                for row in reader:
                    if row[0] == login:
                        return row[1]  # Return the level for the found login
            return None  # If login not found
        except FileNotFoundError:
            return f"CSV file not found at {file_path}."
    
    # Perform the search and display the result
    level = get_level_from_csv(csv_file_path, login_search)
    
    if level:
        print(f"<p>Login: {login_search}, Level: {level}</p>")
    else:
        print(f"<p>No matching login found in the file.</p>")

else:
    print("<p>Please provide both a directory and a login parameter.</p>")

print("""
    </body>
</html>
""")
