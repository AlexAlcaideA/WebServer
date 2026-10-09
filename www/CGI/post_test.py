#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html")
print()

method = os.environ.get('REQUEST_METHOD', 'N/A')
content_type = os.environ.get('CONTENT_TYPE', 'N/A')
content_length = os.environ.get('CONTENT_LENGTH', '0')

print("<h1>CGI POST Test</h1>")
print(f"<p><b>Method:</b> {method}</p>")
print(f"<p><b>Content-Type:</b> {content_type}</p>")
print(f"<p><b>Content-Length:</b> {content_length}</p>")

if method == 'POST':
    length = int(content_length) if content_length else 0
    body = sys.stdin.read(length)
    print(f"<p><b>Body received:</b></p>")
    print(f"<pre>{body}</pre>")
print("</body></html>")
