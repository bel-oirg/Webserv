#!/usr/bin/python3
print("Content-Type: text/html\n")

import os
from urllib.parse import parse_qs

def evaluate_rpn(expression):
    """
    Evaluates an RPN expression.
    """
    try:
        stack = []
        tokens = expression.split()

        for token in tokens:
            if token.isdigit() or (token[0] == '-' and token[1:].isdigit()):
                stack.append(float(token))
            else:
                # Perform operation
                if len(stack) < 2:
                    return "Error: Insufficient values for operation"
                b = stack.pop()
                a = stack.pop()

                if token == '+':
                    stack.append(a + b)
                elif token == '-':
                    stack.append(a - b)
                elif token == '*':
                    stack.append(a * b)
                elif token == '/':
                    if b == 0:
                        return "Error: Division by zero"
                    stack.append(a / b)
                else:
                    return f"Error: Invalid operator '{token}'"

        if len(stack) != 1:
            return "Error: Invalid RPN expression"

        return stack[0]
    except Exception as e:
        return f"Error: {str(e)}"

def print_response(result=None, error=None, first_time=False):
    """
    Prints the HTML response with a form for new input.
    """
    print(f"""
    <html>
    <head>
        <title>RPN Calculator</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                background-color: #f4f4f9;
                padding: 20px;
                display: flex;
                flex-direction: column;
                align-items: center;
            }}
            h1 {{ color: #4CAF50; }}
            form {{
                background-color: white;
                padding: 20px;
                border-radius: 8px;
                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
                width: 300px;
                text-align: center;
            }}
            input[type="text"] {{
                width: calc(100% - 20px);
                padding: 10px;
                margin: 10px 0;
                font-size: 16px;
            }}
            input[type="submit"] {{
                background-color: #4CAF50;
                color: white;
                border: none;
                padding: 10px;
                width: 100%;
                cursor: pointer;
                font-size: 16px;
                margin-top: 10px;
            }}
            .result, .error {{
                margin-top: 20px;
                padding: 10px;
                font-size: 18px;
                text-align: center;
                width: calc(100% - 40px);
                border-radius: 5px;
            }}
            .result {{ background-color: #e8f5e9; color: #388e3c; }}
            .error {{ background-color: #ffebee; color: #d32f2f; }}
        </style>
    </head>
    <body>
        <h1>RPN Calculator</h1>
        <form method="GET">
            <input type="text" name="expression" placeholder="Enter RPN expression (e.g., '3 4 +')" required>
            <input type="submit" value="Calculate">
        </form>
    """)

    if not first_time:
        if error:
            print(f"<div class='error'>{error}</div>")
        elif result is not None:
            print(f"<div class='result'>Result: {result}</div>")
    
    print("</body></html>")

# Main logic for handling CGI input
def main():
    # Get QUERY_STRING from the environment
    query_string = os.environ.get("QUERY_STRING", "")

    # If the query string is empty, assume it's the first time the CGI is called
    if not query_string:
        print_response(first_time=True)
        return

    # Parse the query string
    params = parse_qs(query_string)
    expression = params.get('expression', [None])[0]

    if not expression:
        print_response(error="Missing RPN expression. Please enter a valid expression.")
        return

    # Evaluate the RPN expression
    result = evaluate_rpn(expression)

    # Check for errors in evaluation
    if isinstance(result, str) and result.startswith("Error"):
        print_response(error=result)
    else:
        print_response(result=result)

if __name__ == "__main__":
    main()
