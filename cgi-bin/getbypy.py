import requests

response = requests.get("http://127.0.0.1:8003/cgi/getbypy.py")
print(response.text)  # Print the response body
