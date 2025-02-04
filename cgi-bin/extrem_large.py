import sys

def main():
    print("Content-Type: text/plain\n")  # CGI header
    print()  # Blank line to separate headers from body
    
    # Generate an extremely large output
    for i in range(10**6):  # 1 billion lines
        print(f"Hello, World! {i}")
        
        # Flush output to avoid buffering delays
        if i % 10000 == 0:
            sys.stdout.flush()

if __name__ == "__main__":
    main()