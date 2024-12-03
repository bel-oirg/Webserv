import os

for param in os.environ.keys():
    print ("<h4> %s :   ---> %s </h4>" % (param, os.environ[param]))