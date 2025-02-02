import http.client

def chunked_encode(data, chunk_size=5):
    """Convert data into HTTP chunked encoding format."""
    index = 0
    while index < len(data):
        chunk = data[index:index + chunk_size]
        yield f"{len(chunk):X}\r\n{chunk}\r\n".encode()
        index += chunk_size
    yield b"0\r\n\r\n"  # End of chunked encoding

def send_chunked_request(host, port, path, data):
    """Send an HTTP request with chunked encoding."""
    conn = http.client.HTTPConnection(host, port)
    headers = {
        "Transfer-Encoding": "chunked",
        "Content-Type": "text/plain"
    }
    conn.putrequest("POST", path)
    
    for key, value in headers.items():
        conn.putheader(key, value)
    conn.endheaders()

    for chunk in chunked_encode(data):
        conn.send(chunk)

    response = conn.getresponse()
    print(f"Response Status: {response.status}")
    print(f"Response Data: {response.read().decode()}")
    conn.close()

# Usage
send_chunked_request("localhost", 8080, "/upload", "Hello, this is a chunked request test!")
